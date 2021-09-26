#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>

struct RequestInfo {
    std::string method;
    std::string path;
    std::string http_version;
};

struct Transaction {
    RequestInfo request;
    std::vector<std::string> headers;
    std::string body;
};

#endif
