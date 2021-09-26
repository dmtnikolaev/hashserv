#include <iostream>
#include <unistd.h>

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
}

#include "transparser.h"
#include "transaction.h"
#include "processing.h"

std::string BuildResponce(const std::string& process_result) {
    std::string resp = "HTTP/1.0 200 OK\r\n"
    "Content-Type: application/json\r\n\r\n" +
        process_result;

    resp += "\r\n";
    return resp;
}

int main() {
    TransParser parser;
    
    constexpr int kBacklog = 10;
    constexpr int kPort = 8001;
    constexpr int kBuffSize = 2048;

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
    int client_socket;

    char data[kBuffSize];
    while (true) {
        client_socket = accept(server_socket, nullptr, nullptr);
        auto recieved = recv(client_socket, data, kBuffSize, 0);
        auto trans = parser.ParseTransaction(std::string(data, recieved));
        auto resp = BuildResponce(ProcessTransaction(trans));
        std::cout << resp << std::endl;
        send(client_socket, resp.c_str(), resp.length(), 0);
        close(client_socket);
    }

    return 0;
}