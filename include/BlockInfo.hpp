#pragma once

#include <string>
#include <vector>

struct BlockInfo {
    int index;
    std::string previousHash;
    std::string originalBlockHash;
    std::string nonce;
    std::vector<std::string> originalData;
    // data[0] is the string to attack
    // data[1] (if present) is the original entropy string
};