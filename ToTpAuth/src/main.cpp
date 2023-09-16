#include <iostream>
#include "Totp.h"
#include <ctime>

int main() {

    std::string strSecret = "VYD6WZQ6GGK6EDS2";
    std::time_t t = time(nullptr);
    Totp totp(strSecret);
    totp.genTotp(t);

    return 0;
}
