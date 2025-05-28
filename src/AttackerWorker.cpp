#include "AttackerWorker.hpp"
#include "TransactionGenerator.hpp"
#include "PermutationGenerator.hpp"
#include "common_attacker.hpp" // Убедитесь, что эти файлы существуют и содержат
#include "hash_attacker.hpp" // getMerkleRoot_OriginalFlawed и sha256_attacker
#include "Logging.hpp"
#include "Globals.hpp"
#include "console_colors.hpp"
#include "BlockInfo.hpp"
#include <vector>
#include <functional>
#include <mutex>

void attacker_worker_function(
        int thread_id_for_log,
        const BlockInfo& target_block,
        const AttackerConfig& config,
        unsigned long long start_perm_index,
        unsigned long long num_perms_for_thread) {

    std::function<std::string(unsigned long long)> get_permutation_func;

    try {
        if (config.attackMode) {
            TransactionGenerator generator(config.addressCharSet, config.amountCharSet,
                                           config.senderAddressLength, config.receiverAddressLength,
                                           config.amountLength);
            get_permutation_func = [generator](unsigned long long k) { return generator.getPermutation(k); };
        } else { // "permutation"
            PermutationGenerator generator(config.charSet, config.dataLength);
            get_permutation_func = [generator](unsigned long long k) { return generator.getPermutation(k); };
        }
    } catch (const std::exception& e) {
        safe_log("Thread " + std::to_string(thread_id_for_log) + " Error initializing generator: " + e.what(), Color::RED);
        return;
    }

    unsigned long long permutations_checked_by_thread = 0;
    unsigned long long log_interval = (num_perms_for_thread > 200) ? (num_perms_for_thread / 100) : 0;

    for (unsigned long long i = 0; i < num_perms_for_thread; ++i) {
        if (g_solution_found.load()) {
            return;
        }

        unsigned long long current_index = start_perm_index + i;
        std::string current_data_permutation_string;
        try {
            current_data_permutation_string = get_permutation_func(current_index);
        } catch (const std::exception& e) {
            safe_log("Thread " + std::to_string(thread_id_for_log) + " Error: Failed to get permutation " + std::to_string(current_index) + ": " + e.what(), Color::RED);
            continue;
        }

        std::vector<std::string> data_for_merkle_root;
        if (!target_block.originalData.empty()) {
            data_for_merkle_root = target_block.originalData;
            if (data_for_merkle_root.empty()) { data_for_merkle_root.push_back(current_data_permutation_string); }
            else { data_for_merkle_root[0] = current_data_permutation_string; }
        } else { data_for_merkle_root.push_back(current_data_permutation_string); }
        if (!config.useOriginalEntropy) {
            if (data_for_merkle_root.size() == 1 && !config.attackEntropyValue.empty()) { data_for_merkle_root.push_back(config.attackEntropyValue); }
            else if (data_for_merkle_root.size() > 1) { data_for_merkle_root[1] = config.attackEntropyValue; }
            else if (data_for_merkle_root.size() == 0 && !config.attackEntropyValue.empty()) { data_for_merkle_root.push_back(current_data_permutation_string); data_for_merkle_root.push_back(config.attackEntropyValue); }
        }
        if (data_for_merkle_root.empty()) { data_for_merkle_root.push_back(current_data_permutation_string); }

        std::string current_merkle_root = getMerkleRoot_OriginalFlawed(data_for_merkle_root);
        std::string header_base = std::to_string(target_block.index) + target_block.previousHash + current_merkle_root;
        std::string string_to_hash = header_base + target_block.nonce;
        std::string new_hash = sha256_attacker(string_to_hash);

        permutations_checked_by_thread++;
        g_permutations_checked_total++;

        bool match_found = (new_hash == target_block.originalBlockHash);

        if (match_found) {
            std::lock_guard<std::mutex> lock(g_result_mutex);
            if (!g_solution_found.load()) {
                g_solution_found.store(true);
                g_found_data_string = current_data_permutation_string;
                g_found_hash = new_hash;
                g_found_block_index = target_block.index;
                g_found_original_hash = target_block.originalBlockHash;

                std::string success_msg_body = "Match Found by Thread " + std::to_string(thread_id_for_log) + "!\n" +
                                               "  Block Index:       " + std::to_string(target_block.index) + "\n" +
                                               "  Original Hash:     " + target_block.originalBlockHash + "\n" +
                                               "  Found Data[0]:     '" + current_data_permutation_string + "'\n" +
                                               "  Calculated Hash:   " + new_hash;
                safe_log(success_msg_body, Color::GREEN + Color::BOLD);
            }
            return;
        }

        if (log_interval > 0 && (permutations_checked_by_thread % log_interval == 0)) {
            double perc = static_cast<double>(permutations_checked_by_thread) * 100.0 / num_perms_for_thread;
            safe_log("Thread " + std::to_string(thread_id_for_log) + " (Block " + std::to_string(target_block.index) + "): " + std::to_string(static_cast<int>(perc)) + "% checked.", Color::CYAN);
        }
    }
}