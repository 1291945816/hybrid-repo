#include <iostream>
#include "Totp.h"
#include <ctime>

int main() {
    /*例子*/
    std::string strSecret = "VYD6WZQ6GGK6EDS2";
    std::time_t t = time(nullptr);
    Totp totp(strSecret);
    auto code = totp.genTotp(t);
    std::cout << code << "\n";

    return 0;
}
