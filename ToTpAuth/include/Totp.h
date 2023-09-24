
/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2023/9/15 12:29
* @version: 1.0
* @description: 
********************************************************************************/

#pragma once
#ifndef TOTPAUTH_TOTP_H
#define TOTPAUTH_TOTP_H
#include <iostream>
#include <string>
#include <sha.h>  // 加密库
#include <chrono>


class Totp {
    /*成员函数*/
public:
    /**
     * 构建一个Totp对象
     * @param secret 密钥 采用base32进行加密的
     * @param initTime 初始时间  默认为 0
     * @param timeStep 时间间隔  默认为 30
     * @param digit 位数        默认为 6
     */
    explicit Totp(std::string& secret,
                  long long int initTime = 0L,
                  long long int timeStep=30L,unsigned int digit=6):strSecret_(std::move(secret)),
                                                                    initTime_(initTime),
                                                                    timeStep_(timeStep),
                                                                    digit_(digit){}

    std::string genTotp(std::time_t currTime) ;
    ~Totp()=default;

private:
    std::string strSecret_;  // 密钥
    long long int initTime_; // 起始时间
    long long int timeStep_; //时间步长
    unsigned int digit_; // TOTP码的位数



      std::string getHmac(long long count);


    /*类工具函数 仅供类内使用*/
    static CryptoPP::byte *  longLongToBytes(long long int dig);
    static CryptoPP::byte hexCharToByte(char c);
    static CryptoPP::byte * hexStringToByteArray(const std::string& hexString);
    static std::string decodeSecret(const std::string & strSecret);

};


#endif //TOTPAUTH_TOTP_H
