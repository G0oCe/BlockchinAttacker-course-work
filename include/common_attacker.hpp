#pragma once
#include <string>
#include <vector>
#include "hash_attacker.hpp" // For sha256_attacker

inline std::string getMerkleRoot_OriginalFlawed(const std::vector<std::string>& merkle_data_input) {
    // std::cout << "\nFinding Merkle Root (OriginalFlawed Logic).... \n"; // Keep commented for performance

    if (merkle_data_input.empty()) {
        return ""; // Or hash of empty string if that's the convention
    }
    if (merkle_data_input.size() == 1) {
        return sha256_attacker(merkle_data_input[0]); // Assuming sha256_attacker matches common.hpp's sha256
    }

    std::vector<std::string> current_processing_level = merkle_data_input;

    while (current_processing_level.size() > 1) {
        if (current_processing_level.size() % 2 == 1) {
            current_processing_level.push_back(merkle_data_input.back());
        }


        std::vector<std::string> next_level_hashes;
        for (size_t i = 0; i < current_processing_level.size(); i += 2) {
            std::string var1 = sha256_attacker(current_processing_level[i]);
            std::string var2 = sha256_attacker(current_processing_level[i+1]);
            std::string combined_hash = sha256_attacker(var1 + var2);
            next_level_hashes.push_back(combined_hash);
        }
        current_processing_level = next_level_hashes;
    }
    return current_processing_level[0];
}