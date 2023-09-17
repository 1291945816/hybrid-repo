#include <iostream>
#include "Totp.h"
#include <ctime>

int main() {

    /*
     * TODO
     * 将根据配置文件来读取相关信息
     */
    std::string strSecret = "VYD6WZQ6GGK6EDS2";
    std::time_t t = time(nullptr);
    Totp totp(strSecret,0L,30L,8);
    auto code = totp.genTotp(t);
    std::cout << code << "\n";

    return 0;
}
