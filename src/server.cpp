#include "server.h"

#include <unistd.h>
#include <iostream>

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
}

#include "pool.h"
#include "transparser.h"
#include "processing.h"

bool Server::Start() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(
        server_socket_, 
        (sockaddr *)& server_address,
        sizeof(server_address)
    );

    int is_listening = listen(server_socket_, kBacklog_);
    if (is_listening < 0) { 
        return false;
    }
    
    ListeningLoop();

    return true;
}

void Server::ListeningLoop() {
    auto pool = Pool(kPoolSize_);

    std::cout << "Server listening on http://172.0.0.1:";
    std::cout << port_ << std::endl;

    while (true) {
        auto client_socket = accept(server_socket_, nullptr, nullptr);
        pool.AddJob(std::make_tuple(&ProcessTransaction, client_socket));
    }
}

void ProcessTransaction(int client_socket) {
    char data[kTransBuffSize];
    auto recieved = recv(client_socket, data, kTransBuffSize, 0);
    if (recieved == 0) {
        return;
    }

    TransParser parser;
    auto request = parser.ParseTransaction(std::string(data, recieved));
    Status status;
    auto result = ProcessRequest(request, &status);
    auto responce = BuildResponceStr(result, status);
    send(client_socket, responce.c_str(), responce.length(), 0);
    close(client_socket);
}

std::string BuildResponceStr(const std::string& process_result, Status status) {
    std::string resp;
    if (status == Status::OK) {
        resp = "HTTP/1.0 200 OK\r\n";
        if (process_result.length() > 0) {
            resp += "Content-type: application/json\r\n\r\n";
            resp += process_result;
        }
    }
    else if (status == Status::NOT_FOUND) {
        resp = "HTTP/1.0 404 Not Found\r\n";
    }
    else {
        resp = "";
    }

    resp += "\r\n";
    return resp;
}