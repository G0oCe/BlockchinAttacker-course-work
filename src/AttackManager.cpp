#include "AttackManager.hpp"
#include "AttackerWorker.hpp"
#include "Globals.hpp"
#include "Logging.hpp"
#include "Utils.hpp"
#include "TransactionGenerator.hpp"
#include "PermutationGenerator.hpp"
#include "console_colors.hpp"
#include "BlockInfo.hpp"
#include "AttackerConfig.hpp"

#include <thread>
#include <vector>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <numeric>
#include <stdexcept>

inline bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

AttackManager::AttackManager(const AttackerConfig& cfg, const std::vector<BlockInfo>& all_blocks_info)
        : config(cfg), all_blocks(all_blocks_info) {

    std::string base_name = config.nTimesResultsFilePath + std::to_string(config.numberOfAttacks) + "_times";
    std::string extension = ".txt";
    std::string current_path = base_name + extension;
    int counter = 0;

    // Пока файл с текущим именем существует, увеличиваем счетчик и пробуем новое имя
    while (file_exists(current_path)) {
        counter++;
        std::ostringstream ss;
        ss << base_name << "(" << counter << ")" << extension;
        current_path = ss.str();
    }

    this->n_times_file_path = current_path;

    if (config.attackTarget == "last" && !all_blocks.empty()) {
        target_blocks_info.push_back(all_blocks.back());
    } else if (config.attackTarget == "all") {
        target_blocks_info = all_blocks;
    } else {
        safe_log("WARN: Unsupported attackTarget '" + config.attackTarget + "'. Using 'last'.", Color::YELLOW);
        if (!all_blocks.empty()) {
            target_blocks_info.push_back(all_blocks.back());
        }
    }

    // Просто создаем/очищаем файл. Если не удается, выбрасываем исключение.
    std::ofstream n_times_out(this->n_times_file_path, std::ios::trunc);
    if (!n_times_out) {
        safe_log("FATAL ERROR: Could not open " + this->n_times_file_path + " for writing.", Color::RED + Color::BOLD);
        throw std::runtime_error("Could not open " + this->n_times_file_path);
    }
}

unsigned long long AttackManager::get_total_permutations() {
    try {
        if (config.attackMode) {
            TransactionGenerator temp_gen(config.addressCharSet, config.amountCharSet, config.senderAddressLength, config.receiverAddressLength, config.amountLength);
            return temp_gen.getTotalPermutations();
        } else {
            PermutationGenerator temp_gen(config.charSet, config.dataLength);
            return temp_gen.getTotalPermutations();
        }
    } catch (const std::exception& e) {
        safe_log("FATAL ERROR initializing Generator: " + std::string(e.what()), Color::RED + Color::BOLD);
        throw;
    }
}

bool AttackManager::run_single_attack(const BlockInfo& block_to_attack, unsigned long long total_perms) {
    auto block_start_time = std::chrono::high_resolution_clock::now();
    safe_log("  -> Attacking Block Index: " + std::to_string(block_to_attack.index) + " ...", Color::CYAN);

    std::vector<std::thread> threads;
    unsigned int num_threads = config.useMultithreading ? config.numThreads : 1;
    if (total_perms < num_threads && total_perms > 0) { num_threads = static_cast<unsigned int>(total_perms); }
    if (total_perms == 0) num_threads = 1;

    unsigned long long perms_per_thread = (total_perms > 0) ? (total_perms / num_threads) : 0;
    unsigned long long remainder = (total_perms > 0) ? (total_perms % num_threads) : 0;
    if (total_perms == 1 && num_threads == 1) { perms_per_thread = 1; remainder = 0; }

    unsigned long long current_start_idx = 0;
    g_permutations_checked_total.store(0);

    for (unsigned int i = 0; i < num_threads; ++i) {
        unsigned long long count_for_this = perms_per_thread + (i < remainder ? 1 : 0);
        if (count_for_this == 0 && total_perms > 0) continue;
        if (total_perms == 0) count_for_this = 1;

        threads.emplace_back(attacker_worker_function, i + 1, std::cref(block_to_attack), std::cref(config), current_start_idx, count_for_this);
        current_start_idx += count_for_this;
    }

    for (auto& t : threads) { if (t.joinable()) t.join(); }

    auto block_end_time = std::chrono::high_resolution_clock::now();
    auto block_duration = std::chrono::duration_cast<std::chrono::milliseconds>(block_end_time - block_start_time);
    bool found = g_solution_found.load();
    std::string status = found ? "Solution FOUND" : "No solution found";
    std::string color = found ? Color::GREEN + Color::BOLD : Color::YELLOW;
    safe_log("  -> Attack on Block Index: " + std::to_string(block_to_attack.index) + " - " + status +
             ". Checked: " + std::to_string(g_permutations_checked_total.load()) +
             ". Duration: " + format_duration(block_duration), color);

    return found;
}

void AttackManager::write_summary_file(const std::vector<std::chrono::milliseconds>& durations, int successful_runs) {
    std::ofstream n_times_out(this->n_times_file_path, std::ios::trunc); // Перезаписываем файл
    if (!n_times_out) {
        safe_log("ERROR: Could not write summary to " + this->n_times_file_path, Color::RED);
        return;
    }

    long long total_ms = 0;
    for (const auto& d : durations) { total_ms += d.count(); }
    double avg_ms = (durations.empty()) ? 0.0 : static_cast<double>(total_ms) / durations.size();

    n_times_out << "===== Summary =====\n";

    // <<< НАЧАЛО ДОБАВЛЕННОГО КОДА >>>
    n_times_out << "Threads:           " << config.numThreads << "\n";

    if (config.attackMode == 0) { // Режим перебора (Permutation)
        n_times_out << "Attack Mode:       Permutation (0)\n";
        n_times_out << "Data Length:       " << config.dataLength << "\n";
        n_times_out << "Char Set:          '" << config.charSet << "'\n";
    } else { // Режим транзакций (Transaction)
        n_times_out << "Attack Mode:       Transaction (1)\n";
        n_times_out << "Address Char Set:  '" << config.addressCharSet << "'\n";
        n_times_out << "Amount Char Set:   '" << config.amountCharSet << "'\n";
        n_times_out << "Amount Length:     " << config.amountLength << "\n";
        n_times_out << "Sender Addr Len:   " << config.senderAddressLength << "\n";
        n_times_out << "Receiver Addr Len: " << config.receiverAddressLength << "\n";
    }
    n_times_out << "---------------------\n"; // Разделитель для наглядности
    // <<< КОНЕЦ ДОБАВЛЕННОГО КОДА >>>

    n_times_out << "Total Runs:        " << config.numberOfAttacks << "\n";
    n_times_out << "Successful Runs:   " << successful_runs << "\n";
    n_times_out << "Average Duration:  " << std::fixed << std::setprecision(2) << avg_ms << " ms\n";
    n_times_out << "duration: (";
    for (size_t i = 0; i < durations.size(); ++i) {
        n_times_out << durations[i].count() << (i == durations.size() - 1 ? "" : " + ");
    }
    n_times_out << ") / " << durations.size() << " = " << std::fixed << std::setprecision(2) << avg_ms << " ms\n";
    n_times_out << "=====================\n";
}

void AttackManager::run_attacks() {
    if (target_blocks_info.empty()) {
        safe_log("No blocks selected for attack. Exiting.", Color::YELLOW);
        return;
    }

    unsigned long long total_perms = get_total_permutations();
    safe_log("Total combinations to check per targeted block: " + std::to_string(total_perms), Color::BLUE);

    std::vector<std::chrono::milliseconds> attack_durations;
    int successful_runs_count = 0;

    for (int run = 1; run <= config.numberOfAttacks; ++run) {
        safe_log("==========================================================", Color::YELLOW + Color::BOLD);
        safe_log("Starting Attack Run: " + std::to_string(run) + "/" + std::to_string(config.numberOfAttacks), Color::YELLOW + Color::BOLD);

        g_solution_found.store(false);
        g_found_data_string = ""; g_found_hash = "";
        g_found_original_hash = ""; g_found_block_index = -1;

        auto run_start_time = std::chrono::high_resolution_clock::now();
        bool run_found_solution = false;

        for (const auto& block_to_attack : target_blocks_info) {
            if (run_single_attack(block_to_attack, total_perms)) {
                run_found_solution = true;
                break;
            }
        }

        auto run_end_time = std::chrono::high_resolution_clock::now();
        auto run_duration = std::chrono::duration_cast<std::chrono::milliseconds>(run_end_time - run_start_time);
        attack_durations.push_back(run_duration);

        if (run_found_solution) {
            successful_runs_count++;
            safe_log("Run " + std::to_string(run) + " SUCCESSFUL. Duration: " + format_duration(run_duration), Color::GREEN + Color::BOLD);
        } else {
            safe_log("Run " + std::to_string(run) + " FAILED (No match). Duration: " + format_duration(run_duration), Color::RED);
        }
    }

    write_summary_file(attack_durations, successful_runs_count);
    safe_log("N-Times Attack run finished. Results in " + this->n_times_file_path, Color::MAGENTA + Color::BOLD);
}