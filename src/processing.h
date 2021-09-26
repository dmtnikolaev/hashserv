#ifndef PROCESSING_H
#define PROCESSING_H

#include "transaction.h"

#include "mhash.h"
#include "server.h"

const unsigned char kDigitMap[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'
};

enum Method {
    HEAD,
    POST,
    GET,
    PUT,
    PATH,
    DELETE,
    UNREALIZED,
    UNKNOWN
};

const std::string kMethodStrs[] = {
    "HEAD",
    "POST",
    "GET",
    "PUT",
    "PATH",
    "DELETE",
    "UNREALIZED",
    "UNKNOWN"
};

std::string ProcessRequest(const Transaction& trans, Status* status);

Method DetermineMethod(const RequestInfo& info);

std::string HashData(const std::string& data, hashid hash_method);

#endif