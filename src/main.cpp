#include "server.h"

int main() {
    auto serv = Server(8001);
    serv.Start();
    return 0;
}