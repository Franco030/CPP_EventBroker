#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <thread>
#include <orm/Database.hpp>

namespace broker {

class TcpServer {
public:
    TcpServer(int port, orm::Database& db);
    ~TcpServer();

    void start();
    void stop();

private:
    int port_;
    int server_fd_;
    bool running_;
    orm::Database& db_;
    
    std::mutex subscribers_mutex_;
    std::unordered_map<int, std::vector<int>> topic_subscribers_;

    void handleClient(int client_socket);
};

} // namespace broker
