#include "../src/transparser.h"

TEST(TransParserTest, ParseTransOnlyReq) {
    TransParser parser;
    std::string raw_trans = "POST / HTTP/1.0\r\n\r\n";
    auto res = parser.ParseTransaction(raw_trans);

    ASSERT_STREQ(res.request.method.c_str(), "POST");
    ASSERT_STREQ(res.request.path.c_str(), "/");
    ASSERT_STREQ(res.request.http_version.c_str(), "HTTP/1.0");
}

TEST(TransParserTest, ParseTransBig) {
    TransParser parser;
    std::string raw_trans = "POST / HTTP/1.0\r\nHost: 127.0.0.1:8001\r\n"
        "User-Agent: curl/7.68.0\r\nAccept: */*\r\n"
        "Content-Type: application/json\r\nContent-Length: 17\r\n\r\n"
        "{\"data\": \"linux\"}";

    auto res = parser.ParseTransaction(raw_trans);

    ASSERT_STREQ(res.request.method.c_str(), "POST");
    ASSERT_STREQ(res.request.path.c_str(), "/");
    ASSERT_STREQ(res.request.http_version.c_str(), "HTTP/1.0");

    ASSERT_STREQ(res.body.c_str(), "{\"data\": \"linux\"}");
}