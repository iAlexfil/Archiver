#pragma once

#include <bitset>
#include <iostream>
#include <cstdint>

const std::size_t BLOCK_SIZE = 9;
const std::size_t MAX_CODE_SIZE = 64;
const std::bitset<BLOCK_SIZE> NEUTRAL("000000000");
const std::bitset<BLOCK_SIZE> FILENAME_END("100000000");
const std::bitset<BLOCK_SIZE> ONE_MORE_FILE("100000001");
const std::bitset<BLOCK_SIZE> ARCHIVE_END("100000010");

bool operator<(const std::bitset<BLOCK_SIZE> a, const std::bitset<BLOCK_SIZE> b) {
    for (std::size_t i = BLOCK_SIZE; i > 0; i--) {
        if (a[i - 1] ^ b[i - 1]) {
            return b[i - 1];
        }
    }
    return false;
}

bool operator<(std::tuple<int32_t, std::bitset<BLOCK_SIZE>, int32_t>& a,
               std::tuple<int32_t, std::bitset<BLOCK_SIZE>, int32_t>& b) {
    if (std::get<0>(a) < std::get<0>(b)) {
        return true;
    }

    if (std::get<0>(a) == std::get<0>(b)) {
        if (std::get<1>(a) < std::get<1>(b)) {
            return true;
        }

        if (std::get<1>(a) == std::get<1>(b)) {
            return std::get<2>(a) < std::get<2>(b);
        }

        return false;
    }

    return false;
}

bool operator<=(std::tuple<int32_t, std::bitset<BLOCK_SIZE>, int32_t>& a,
                std::tuple<int32_t, std::bitset<BLOCK_SIZE>, int32_t>& b) {
    return a == b || a < b;
}

std::bitset<MAX_CODE_SIZE>& operator++(std::bitset<MAX_CODE_SIZE>& a) {
    if (a[0] == false) {
        a[0] = true;
    } else {
        for (std::size_t i = 0; i < BLOCK_SIZE; i++) {
            if (a[i] == true) {
                a[i] = false;
            } else {
                a[i] = true;
                break;
            }
        }
    }
    return a;
}