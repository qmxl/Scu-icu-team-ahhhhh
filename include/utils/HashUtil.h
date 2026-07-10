#ifndef HASH_UTIL_H
#define HASH_UTIL_H

#include <string>
#include <cstdint>

namespace utils {

class HashUtil {
public:
    static std::string sha256(const std::string& input);

private:
    static const uint32_t k[64];
    static uint32_t rotr(uint32_t x, uint32_t n);
    static uint32_t sig0(uint32_t x);
    static uint32_t sig1(uint32_t x);
    static uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
    static void sha256_transform(uint32_t state[8], const uint8_t block[64]);
};

}

#endif