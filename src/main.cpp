#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>
#include <mutex>

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
}

#include "transparser.h"
#include "transaction.h"
#include "processing.h"
#include "pool.h"

constexpr int kBuffSize = 2048;

std::string BuildResponce(const std::string& process_result) {
    std::string resp = "HTTP/1.0 200 OK\r\n"
    "Content-Type: application/json\r\n\r\n" +
        process_result;

    resp += "\r\n";
    return resp;
}

void Responce(int client_socket) {
    TransParser parser;
    char data[kBuffSize];
    auto recieved = recv(client_socket, data, kBuffSize, 0);
    if (recieved == 0) return;
    auto trans = parser.ParseTransaction(std::string(data, recieved));
    auto resp = BuildResponce(ProcessTransaction(trans));
    send(client_socket, resp.c_str(), resp.length(), 0);
    close(client_socket);
}

int main() {
    constexpr int kBacklog = 10;
    constexpr int kPort = 8001;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(kPort);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(
        server_socket, 
        (sockaddr *)& server_address,
        sizeof(server_address)
    );
 
    int listening = listen(server_socket, kBacklog);
    if (listening < 0) {
        std::cout << "Error: The server is not listening." << std::endl;
        return 1;
    }
    
    auto pool = Pool(1024);

    while (true) {
        auto client_socket = accept(server_socket, nullptr, nullptr);
        pool.AddJob(std::make_tuple(&Responce, client_socket));
    }

    return 0;
}