#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <subscribe|publish> [payload_message]" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string message;

    if (mode == "subscribe") {
        message = "{\"type\": \"subscribe\", \"topic_id\": 0}";
    } else if (mode == "publish") {
        std::string payload = (argc > 2) ? argv[2] : "Test message!";
        message = "{\"type\": \"publish\", \"topic_id\": 0, \"payload\": \"" + payload + "\"}";
    } else {
        std::cerr << "Invalid mode. Use 'subscribe' or 'publish'." << std::endl;
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    // Send message
    send(sock, message.c_str(), message.length(), 0);
    std::cout << "Sent: " << message << std::endl;

    // Read response
    char buffer[4096] = {0};
    while (true) {
        ssize_t bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0) {
            std::cout << "Server disconnected." << std::endl;
            break;
        }
        buffer[bytes_read] = '\0';
        std::cout << "Received: " << buffer;
        
        // If we are just publishing, we can exit after the ack
        if (mode == "publish") {
            break;
        }
    }

    close(sock);
    return 0;
}
