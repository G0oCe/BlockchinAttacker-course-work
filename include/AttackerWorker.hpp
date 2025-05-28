#ifndef ATTACKER_WORKER_HPP
#define ATTACKER_WORKER_HPP

#include "BlockInfo.hpp"
#include "AttackerConfig.hpp"
#include <string>

void attacker_worker_function(
        int thread_id_for_log,
        const BlockInfo& target_block,
        const AttackerConfig& config,
        unsigned long long start_perm_index,
        unsigned long long num_perms_for_thread);

#endif // ATTACKER_WORKER_HPP