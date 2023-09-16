
/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2023/9/15 12:29
* @version: 1.0
* @description: 
********************************************************************************/

#include "Totp.h"
#include "base32.h"
#include <filters.h>
#include <hex.h>
#include <hmac.h>
#include <base64.h>




/**
 *
 * @param count
 * @return
 */
auto Totp::getHmac(long long count) {

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
    CryptoPP::StringSource tmpObj(cByte,8,true,hashFilter);  // 不必主动释放内存，因为指针的所有权已经被转移

    delete cByte;

    return calculated_hmac;

}



// 生成Totp码
void Totp::genTotp(std::time_t currTime) {

    long long count = (currTime - initTime_)/timeStep_;  // 确保时间没有问题


    std::string hmac = getHmac(count);
    std::cout << hmac.size() << "\n";


    auto * hmacBytes = hexStringToByteArray(hmac);


    int offset = hmacBytes[19] & 0xf;
    long code =     ((hmacBytes[offset] & 0x7f) << 24) |
                    ((hmacBytes[offset + 1] & 0xff) << 16) |
                    ((hmacBytes[offset + 2] & 0xff) << 8) |
                    (hmacBytes[offset + 3] & 0xff);


    long mod =1e6;
    code = code % mod;
    std::cout << code;


}


/**
 *
 * @param dig
 * @return
 */
CryptoPP::byte* Totp::longLongToBytes(long long int dig) {


    // 小端模式  低地址存低数值
    std::cout << dig << "\n";
    auto bytePtr = new CryptoPP::byte[8]; // 注意处理销毁的问题
    int t = 8;
    for (int i = 0; i < 8; ++i) {
        bytePtr[--t] = dig & 0xFF;
        dig >>= 8; // 右移一个字节
    }
    for (int i = 0; i < 8; ++i) {
        std::cout << (int)(bytePtr[i]) << " ";
    }
    return bytePtr;
}

CryptoPP::byte Totp::hexCharToByte(char c) {
    if (c >= '0' && c <= '9') {
        return static_cast<uint8_t>(c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return static_cast<uint8_t>(c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
        return static_cast<uint8_t>(c - 'A' + 10);
    }
    return 0;
}

CryptoPP::byte *Totp::hexStringToByteArray(const std::string &hexString) {

//    std::cout << hexString << "\n";
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
 *
 * @param strSecret
 * @return
 */
std::string Totp::decodeSecret(const std::string &strSecret) {

    const CryptoPP::byte ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"; // 字母表
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


