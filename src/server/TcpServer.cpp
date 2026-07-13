#include "server/TcpServer.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

#include "models/Event.hpp"
#include <orm/Operations.hpp>

namespace broker {

TcpServer::TcpServer(int port, orm::Database& db) : port_(port), server_fd_(-1), running_(false), db_(db) {}

TcpServer::~TcpServer() { stop(); }

void TcpServer::start() {
  server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd_ == -1) {
    throw std::runtime_error("Failed to create socket");
  }

  int opt = 1;
  // Set socket options to reuse address and port, preventing "Address already in use" errors after restart
  if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    throw std::runtime_error("Failed to set socket options");
  }

  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port_);

  if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
    throw std::runtime_error("Failed to bind to port " + std::to_string(port_));
  }

  if (listen(server_fd_, 10) < 0) {
    throw std::runtime_error("Failed to listen on socket");
  }

  running_ = true;
  std::cout << "TCP Server listening on port " << port_ << std::endl;

  while (running_) {
    sockaddr_in client_address{};
    socklen_t client_addr_len = sizeof(client_address);

    int client_socket = accept(server_fd_, (struct sockaddr*)&client_address, &client_addr_len);
    if (client_socket < 0) {
      if (running_) {
        std::cerr << "Failed to accept connection" << std::endl;
      }
      continue;
    }

    std::cout << "New client connected! Spawning thread..." << std::endl;
        
    // Handle the client in a separate thread
    std::thread(&TcpServer::handleClient, this, client_socket).detach();
  }
}

void TcpServer::handleClient(int client_socket) {
    char buffer[4096];
    int subscribed_topic = -1; // Keep track of subscription to clean up on disconnect

    while (running_) {
        ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break; // Connection closed or error
        }
        
        // Null-terminate the string safely
        buffer[bytes_read] = '\0';
        std::string raw_message(buffer);
        
        std::cout << "Received raw bytes: " << raw_message << std::endl;

        try {
            auto j = nlohmann::json::parse(raw_message);
            std::string type = j.value("type", "publish"); // Default to publish if not specified

            if (type == "subscribe") {
                subscribed_topic = j.value("topic_id", 0);
                
                std::lock_guard<std::mutex> lock(subscribers_mutex_);
                topic_subscribers_[subscribed_topic].push_back(client_socket);
                
                std::cout << "Client subscribed to topic " << subscribed_topic << std::endl;
                std::string ack = "{\"status\": \"subscribed\"}\n";
                send(client_socket, ack.c_str(), ack.length(), 0);
            } 
            else if (type == "publish") {
                // Create and populate the Event object
                broker::models::Event event;
                static int next_id = 1;
                event.id = j.value("id", next_id++);
                event.topic_id = j.value("topic_id", 0);
                event.payload = j.value("payload", "");

                std::cout << "Parsed Event -> Topic ID: " << event.topic_id 
                          << ", Payload: " << event.payload << std::endl;
                
                // Store the event using the ORM
                orm::insert(db_, event);
                std::cout << "Successfully saved event to Database!" << std::endl;
                
                std::string ack = "{\"status\": \"ok\"}\n";
                send(client_socket, ack.c_str(), ack.length(), 0);

                // Broadcast to subscribers
                std::lock_guard<std::mutex> lock(subscribers_mutex_);
                if (topic_subscribers_.find(event.topic_id) != topic_subscribers_.end()) {
                    std::string broadcast_msg = j.dump() + "\n";
                    for (int sub_socket : topic_subscribers_[event.topic_id]) {
                        send(sub_socket, broadcast_msg.c_str(), broadcast_msg.length(), 0);
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing/saving event: " << e.what() << std::endl;
            std::string err = "{\"status\": \"error\", \"message\": \"invalid data\"}\n";
            send(client_socket, err.c_str(), err.length(), 0);
        }
    }
    
    // Cleanup on disconnect
    if (subscribed_topic != -1) {
        std::lock_guard<std::mutex> lock(subscribers_mutex_);
        auto& subs = topic_subscribers_[subscribed_topic];
        // Remove this client's socket from the subscriber list
        for (auto it = subs.begin(); it != subs.end(); ) {
            if (*it == client_socket) {
                it = subs.erase(it);
            } else {
                ++it;
            }
        }
    }
    close(client_socket);
}

void TcpServer::stop() {
  running_ = false;
  if (server_fd_ != -1) {
    close(server_fd_);
    server_fd_ = -1;
  }
}

}  // namespace broker
