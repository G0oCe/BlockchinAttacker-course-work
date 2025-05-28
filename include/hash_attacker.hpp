#pragma once

#include <string>
#include <vector> // Usually not needed directly in hash function but good for common_attacker
#include <sstream> // For stringstream
#include <iomanip> // For setw, setfill, hex

#include <openssl/sha.h> // For SHA256 functions

inline std::string sha256_attacker(const std::string& str_to_hash) {
    unsigned char hash_output_buffer[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256_context;

    if (!SHA256_Init(&sha256_context)) {
        throw std::runtime_error("Failed to initialize SHA256 context");
    }
    if (!SHA256_Update(&sha256_context, str_to_hash.c_str(), str_to_hash.length())) {
        throw std::runtime_error("Failed to update SHA256 context");
    }
    if (!SHA256_Final(hash_output_buffer, &sha256_context)) {
        throw std::runtime_error("Failed to finalize SHA256 context");
    }

    std::stringstream hex_string_stream;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hex_string_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash_output_buffer[i]);
    }
    return hex_string_stream.str();
}