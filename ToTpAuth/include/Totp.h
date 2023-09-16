
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


// 这里的函数推断有点问题


#include <iostream>
#include <sha.h>  // 加密库
#include <chrono>
class Totp {
    /*成员函数*/
public:
    explicit Totp(std::string& secret,long long int initTime = 0,long long int timeStep=30 ):strSecret_(std::move(secret)),
                                                                    initTime_(initTime),
                                                                    timeStep_(timeStep){}

    void genTotp(std::time_t currTime);
    ~Totp()=default;
private:
    std::string strSecret_;  // 密钥
    long long int initTime_; // 起始时间
    long long int timeStep_; //时间步长



    auto getHmac(long long count);
    /*类工具函数 仅供类内使用*/
    static CryptoPP::byte *  longLongToBytes(long long int dig);
    static CryptoPP::byte hexCharToByte(char c);
    static CryptoPP::byte * hexStringToByteArray(const std::string& hexString);
    static std::string decodeSecret(const std::string & strSecret);




};


#endif //TOTPAUTH_TOTP_H
