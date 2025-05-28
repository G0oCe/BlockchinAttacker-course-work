#include <iostream>
#include <vector>
#include <chrono>


#include "AttackerConfig.hpp"
#include "Logging.hpp"
#include "Utils.hpp"
#include "BlockchainParser.hpp"
#include "BlockInfo.hpp"
#include "AttackManager.hpp"
#include "console_colors.hpp"



int main(int argc, char* argv[]) {
    Color::enableVirtualTerminalProcessing();
    auto overall_start_time = std::chrono::high_resolution_clock::now();

    std::string config_file_path = "../../config_attacker.json";
    if (argc > 1) { config_file_path = argv[1]; }
    std::cout << "Using config file: " << config_file_path << std::endl;

    AttackerConfig config;
    try {
        config = load_attacker_config(config_file_path);
    } catch (const std::exception& e) {
        std::cerr << Color::RED << Color::BOLD << "FATAL ERROR (Config Load): " << e.what() << Color::RESET << std::endl;
        return 1;
    }

    init_logging(config.logFilePath, config.resultsFilePath);
    safe_log("==========================================================", Color::MAGENTA + Color::BOLD);
    safe_log("Attacker Started (Manager Mode)", Color::MAGENTA + Color::BOLD);
    safe_log("N-Runs: " + std::to_string(config.numberOfAttacks), Color::BLUE);

    BlockchainParser parser;
    std::vector<BlockInfo> all_blocks;
    try {
        safe_log("Attempting to load blockchain from: " + config.blockchainFilePath, Color::CYAN);
        all_blocks = parser.parseBlockchain(config.blockchainFilePath);
        safe_log("Successfully loaded " + std::to_string(all_blocks.size()) + " blocks.", Color::GREEN);
    } catch (const std::exception& e) {
        safe_log("Fatal Error parsing blockchain: " + std::string(e.what()), Color::RED + Color::BOLD);
        close_logging();
        return 1;
    }

    try {
        AttackManager manager(config, all_blocks);
        manager.run_attacks();
    } catch (const std::exception& e) {
        safe_log("FATAL ERROR during attack execution: " + std::string(e.what()), Color::RED + Color::BOLD);
        close_logging();
        return 1;
    }

    auto overall_end_time = std::chrono::high_resolution_clock::now();
    auto overall_duration = std::chrono::duration_cast<std::chrono::milliseconds>(overall_end_time - overall_start_time);
    safe_log("==========================================================", Color::MAGENTA + Color::BOLD);
    safe_log("Attacker Run Completed. Total Duration: " + format_duration(overall_duration), Color::MAGENTA + Color::BOLD);

    close_logging();
    return 0;
}