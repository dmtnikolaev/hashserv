#include "transparser.h"

#include <vector>
#include "transaction.h"

Transaction TransParser::ParseTransaction(const std::string& raw_trans) {
    std::string raw_info;
    std::vector<std::string> raw_headers;
    std::string raw_body;
    
    auto pos = 0;
    auto new_line = false;
    auto car_ret = false;

    for (int i = 0; i < (int)raw_trans.length(); i++) {
        if (raw_trans[i] == '\r') {
            car_ret = true;
        }
        else if (raw_trans[i] == '\n' && car_ret) {
            if (new_line) {
                pos = i + 1;
                break;
            }

            if (pos == 0) {
                raw_info = raw_trans.substr(pos, i - 1 - pos);
            }
            else {
                raw_headers.push_back(raw_trans.substr(pos, i - 1 - pos));
            }
            pos = i + 1;
            new_line = true;
            car_ret = false;
        }
        else {
            new_line = false;
            car_ret = false;
        }
    }

    raw_body = raw_trans.substr(pos, raw_trans.length() - pos);
    
    auto info = ParseInfo(raw_info);

    Transaction tr {info, raw_headers, raw_body};

    return tr;
}

RequestInfo TransParser::ParseInfo(std::string raw_request) {
    std::vector<std::string> req_tokens;
    auto pos = 0;

    for (auto i = 0; i < (int)raw_request.length(); i++) {
        if (raw_request[i] == ' ') {
            req_tokens.push_back(raw_request.substr(pos, i - pos));
            pos = i + 1;
        }
    }
    req_tokens.push_back(raw_request.substr(pos, raw_request.length() - pos));

    RequestInfo info {req_tokens.at(0), req_tokens.at(1), req_tokens.at(2)};

    return info;
}

