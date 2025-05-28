#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept> // For std::runtime_error
#include "BlockInfo.hpp"
#include "nlohmann/json.hpp" // From include/nlohmann/json.hh

using json = nlohmann::json;

class BlockchainParser {
public:
    std::vector<BlockInfo> parseBlockchain(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("BlockchainParser Error: Could not open blockchain file: " + filePath);
        }

        json blockchainJson;
        try {
            file >> blockchainJson;
        } catch (json::parse_error& e) {
            throw std::runtime_error("BlockchainParser Error: JSON parsing failed: " + std::string(e.what()));
        }

        std::vector<BlockInfo> blocks;
        if (!blockchainJson.contains("data") || !blockchainJson["data"].is_array()) {
            if (blockchainJson.contains("length") && blockchainJson["length"].get<int>() == 0) {
                // Empty blockchain, valid case
                return blocks;
            }
            throw std::runtime_error("BlockchainParser Error: 'data' array not found or not an array in JSON.");
        }

        // The structure from your original project seems to be chain["data"][index_as_string]
        // or chain["data"][index_as_int] if it's an array.
        // Let's assume blockchainJson["data"] is an array of block objects.
        // If it's an object with keys "0", "1", etc., the parsing needs adjustment.
        // From your BlockChain.hpp: j["data"][block->getIndex()] = block->toJSON();
        // This implies "data" is an object, not an array. Let's try to handle that.

        if (blockchainJson["data"].is_object()) {
            int num_blocks = 0;
            if (blockchainJson.contains("length") && blockchainJson["length"].is_number_integer()) {
                num_blocks = blockchainJson["length"].get<int>();
            } else {
                // Fallback if length is not present, try to infer from max index in data object
                // This is less robust.
                for (auto it = blockchainJson["data"].begin(); it != blockchainJson["data"].end(); ++it) {
                    try {
                        int current_idx = std::stoi(it.key());
                        if (current_idx >= num_blocks) num_blocks = current_idx + 1;
                    } catch (const std::invalid_argument&){ /* ignore non-integer keys if any */ }
                }
            }

            for (int i = 0; i < num_blocks; ++i) {
                std::string index_str = std::to_string(i);
                if (!blockchainJson["data"].contains(index_str)) {
                    // Skip if a block index is missing, or throw error
                    // For robustness, might log a warning and continue
                    // throw std::runtime_error("BlockchainParser Error: Missing block with index " + index_str);
                    continue;
                }
                const auto& blockJson = blockchainJson["data"][index_str];
                BlockInfo block;
                block.index = blockJson.value("index", -1);
                block.previousHash = blockJson.value("previousHash", "");
                block.originalBlockHash = blockJson.value("hash", "");
                block.nonce = blockJson.value("nonce", "");

                if (blockJson.contains("data") && blockJson["data"].is_array()) {
                    for (const auto& data_item : blockJson["data"]) {
                        if (data_item.is_string()) {
                            block.originalData.push_back(data_item.get<std::string>());
                        }
                    }
                }
                if (block.index == -1 || block.originalBlockHash.empty()) {
                    throw std::runtime_error("BlockchainParser Error: Invalid or incomplete block data for index " + index_str);
                }
                blocks.push_back(block);
            }

        } else if (blockchainJson["data"].is_array()) { // If "data" is an array directly
            for (const auto& blockJson : blockchainJson["data"]) {
                BlockInfo block;
                block.index = blockJson.value("index", -1);
                block.previousHash = blockJson.value("previousHash", "");
                block.originalBlockHash = blockJson.value("hash", "");
                block.nonce = blockJson.value("nonce", "");

                if (blockJson.contains("data") && blockJson["data"].is_array()) {
                    for (const auto& data_item : blockJson["data"]) {
                        if (data_item.is_string()) {
                            block.originalData.push_back(data_item.get<std::string>());
                        }
                    }
                }
                if (block.index == -1 || block.originalBlockHash.empty()) {
                    throw std::runtime_error("BlockchainParser Error: Invalid or incomplete block data in array entry.");
                }
                blocks.push_back(block);
            }
        }



        std::sort(blocks.begin(), blocks.end(), [](const BlockInfo& a, const BlockInfo& b){
            return a.index < b.index;
        });

        return blocks;
    }
};