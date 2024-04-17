
/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2023/9/15 12:29
* @version: 1.0
* @description: 
********************************************************************************/

#include "Totp.h"
#include <base32.h>
#include <filters.h>
#include <hex.h>
#include <hmac.h>




/**
 *采用HMAC-SHA-1计算，生成40个字符的字符串（实际上每两个字符代表一个字节）
 * @param count 计数器
 * @return 计算生成的值
 */
std::string Totp::getHmac(long long count) {

    std::string calculated_hmac;
    auto *cByte = longLongToBytes(count);
    auto * strSingk = new CryptoPP::StringSink(calculated_hmac);
    auto * encoder = new CryptoPP::HexEncoder(strSingk);

    // 解码Base32
    auto decodeSecret_ = decodeSecret(strSecret_);

    // 构造加密
    CryptoPP::HMAC<CryptoPP::SHA1> hmac(reinterpret_cast<const CryptoPP::byte  *>(decodeSecret_.data()),
                                        decodeSecret_.size());
    auto * hashFilter = new CryptoPP::HashFilter(hmac,encoder);
    /*不必主动释放内存，因为指针的所有权已经被转移*/
    CryptoPP::StringSource tmpObj(cByte,8,true,hashFilter);

    delete cByte; // 释放数据

    return calculated_hmac;
}



/**
 *生成TOTP码
 * @param currTime 当前时间戳（Unix时间）
 * @return 空串 代表异常
 */
std::string Totp::genTotp(std::time_t currTime) {

    // TOTP码的要求是最少6位
     if (this->digit_ < 6)
         return "";

    long long count = (currTime - initTime_)/timeStep_;
    std::string hmac = getHmac(count);
    auto * hmacBytes = hexStringToByteArray(hmac);


    int offset = hmacBytes[19] & 0xf;
    long code =     ((hmacBytes[offset] & 0x7f) << 24) |
                    ((hmacBytes[offset + 1] & 0xff) << 16) |
                    ((hmacBytes[offset + 2] & 0xff) << 8) |
                    (hmacBytes[offset + 3] & 0xff);
    long mod = static_cast<long >(std::pow(10,digit_));
    code = code % mod;
     std::string codeStr = std::to_string(code);
     while (codeStr.size() < this->digit_)
        codeStr.insert(0,"0");

    return codeStr;
}


/**
 *将long long类型转换为一个字节数组，
 * Long Long类型是 8字节
 * 大端存储：高地址存低位数据
 * @param dig 待转换的Long类型数据
 * @return
 */
CryptoPP::byte* Totp::longLongToBytes(long long int dig) {


    auto bytePtr = new CryptoPP::byte[8]; // 注意处理销毁的问题
    int t = 8;
    for (int i = 0; i < 8; ++i) {
        bytePtr[--t] = dig & 0xFF;
        dig >>= 8; // 右移一个字节
    }
    return bytePtr;
}


/**
 *将十六进制字符转为字节表示
 * @param c 一个十六进制的字符
 * @return CryptoPP::byte
 */
CryptoPP::byte Totp::hexCharToByte(char c) {
    if (c >= '0' && c <= '9') {
        return static_cast<CryptoPP::byte>(c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return static_cast<CryptoPP::byte>(c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
        return static_cast<CryptoPP::byte>(c - 'A' + 10);
    }
    return 0;
}

/**
 *将十六进制的字符串转换为字节数组
 * @param hexString 十六进制的字符串，应该是偶数，两两为一个字节
 * @return CryptoPP::byte 类型的数组
 */
CryptoPP::byte *Totp::hexStringToByteArray(const std::string &hexString) {

    auto * byteArray = new CryptoPP::byte[hexString.size()/2];
    // 检查输入字符串的长度
    if (hexString.length() % 2 != 0) {
        // 长度不是偶数，无法解析
        return byteArray;
    }

    int t = 0;
    for (size_t i = 0; i < hexString.length(); i += 2) {
        CryptoPP::byte byte = (hexCharToByte(hexString[i]) << 4) | hexCharToByte(hexString[i + 1]);
        byteArray[t++] = byte;
    }
    return byteArray;
}

/**
 *针对采用了Base32算法加密的密钥进行解码
 * CryptoPP库中Base32算法采用的标准是DUDE而不是RFC4648标准，导致解码结果不正确
 * @param strSecret Base32算法加密的密钥
 * @return 解码后的密钥
 */
std::string Totp::decodeSecret(const std::string &strSecret) {


    // RFC4648标准 字母表
    const CryptoPP::byte ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    int lookup[256];
    std::string decodedString;


    CryptoPP::Base32Decoder::InitializeDecodingLookupArray(lookup, ALPHABET, 32, true);
    CryptoPP::AlgorithmParameters params = CryptoPP::MakeParameters(CryptoPP::Name::DecodingLookupArray(),(const int *)lookup);
    auto decoder = new CryptoPP::Base32Decoder(new CryptoPP::StringSink(decodedString));
    decoder->IsolatedInitialize(params);

    CryptoPP::StringSource tmpS(strSecret, true,
                             decoder);

    return decodedString;
}


