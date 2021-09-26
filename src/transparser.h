#ifndef TRANSPARSER_H
#define TRANSPARSER_H

#include "transaction.h"

#include <vector>

class TransParser {
public:
    Transaction ParseTransaction(const std::string& raw_trans);
private:
    RequestInfo ParseInfo(std::string raw_request);
};

#endif
