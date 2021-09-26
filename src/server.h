#ifndef SERVER_H
#define SERVER_H

#include <string>

constexpr int kTransBuffSize = 2048;

enum class Status {
    OK = 200,
    NOT_FOUND = 404
};

class Server {
    public:
        Server(int port) { port_ = port; }
        bool Start();
    private:
        const int kBacklog_ = 10;
        const int kPoolSize_ = 1000;
        
        int port_;
        int server_socket_;

        void ListeningLoop();
};

void ProcessTransaction(int client_socket);
std::string BuildResponceStr(const std::string& process_result, Status status);

#endif
