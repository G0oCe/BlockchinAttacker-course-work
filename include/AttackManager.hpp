#ifndef ATTACK_MANAGER_HPP
#define ATTACK_MANAGER_HPP

#include "AttackerConfig.hpp"
#include "BlockInfo.hpp"
#include <vector>
#include <string>
#include <chrono>

class AttackManager {
public:
    AttackManager(const AttackerConfig& cfg, const std::vector<BlockInfo>& all_blocks_info);
    void run_attacks();

private:
    bool run_single_attack(const BlockInfo& block_to_attack, unsigned long long total_perms);
    void write_summary_file(const std::vector<std::chrono::milliseconds>& durations, int successful_runs);
    unsigned long long get_total_permutations();

    AttackerConfig config;
    std::vector<BlockInfo> all_blocks;
    std::vector<BlockInfo> target_blocks_info;
    std::string n_times_file_path;
};

#endif // ATTACK_MANAGER_HPP