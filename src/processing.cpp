#include "processing.h"

#include "transaction.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "mhash.h"
#include "server.h"

std::string ProcessRequest(const Transaction& trans, Status* status) {
    auto method = DetermineMethod(trans.request);
    std::string res;

    if (method == Method::POST) {
        if (trans.request.path != "/") {
            *status = Status::NOT_FOUND;
            return res;
        }

        if (!CheckHeaders(trans.headers)) {
            *status = Status::OK;
            return res;
        }

        json body;
        try {
            body = json::parse(trans.body);
        }
        catch(std::exception& ex) {
            *status = Status::OK;
            return res;
        }

        res = ProcessHash(body);
        *status = Status::OK;
    }
    else {
        *status = Status::NOT_FOUND;
    }

    return res;
}

Method DetermineMethod(const RequestInfo& info) {
    if (info.method == kMethodStrs[Method::POST]) {
        return Method::POST;
    }
    
    for (const auto& method_str : kMethodStrs) {
        if (info.method == method_str) {
            return Method::UNREALIZED;
        }
    }
    return Method::UNKNOWN;
}

std::string HashData(const std::string& data, hashid hash_method) {
    auto* td = mhash_init(hash_method); 

    unsigned char buffer;
    unsigned char *hash;
            
    for (const char& c : data) {
        buffer = c;
        mhash(td, &buffer, 1);
    }
            
    hash = static_cast<unsigned char *>(mhash_end(td));

    auto bs = mhash_get_block_size(hash_method);
    auto res = std::string();
    res.reserve(2*bs + 1);

    for (auto i = 0; i < (int)bs; i++) {
        int n = static_cast<int>(hash[i]);
        res.push_back(kDigitMap[(n / 16) & 0xf]);
        res.push_back(kDigitMap[n & 0xf]);
    }

    return res;    
}

bool CheckHeaders(const std::vector<std::string>& headers) {
    bool res = false;
    for (const auto& header : headers) {
        if (header == "Content-type: application/json" ||
            header == "Content-Type: application/json") {
            res = true;
        }
    }
    return res;
}

std::string ProcessHash(const json& data) {
    std::string hh;
    std::string res;
    if (data.contains("data")) {
        hh = data["data"].get<std::string>();

        auto h1 = HashData(hh, MHASH_SHA512);
        auto h2 = HashData(hh, MHASH_GOST);

        json hash;
        hash["sha512"] = h1;
        hash["gost"] = h2;

        res = hash.dump();
    }

    return res;
}