#include "AttackerConfig.hpp"
#include "nlohmann/json.hpp" // Убедитесь, что путь к nlohmann/json.hpp правильный
#include <fstream>
#include <stdexcept>
#include <thread>

AttackerConfig load_attacker_config(const std::string& config_path) {
    std::ifstream f(config_path);
    if (!f.is_open()) { throw std::runtime_error("FATAL ERROR: Could not open attacker config file: " + config_path); }

    nlohmann::json cfg_json;
    try { f >> cfg_json; }
    catch (const nlohmann::json::exception& e) { throw std::runtime_error("FATAL ERROR: Failed to parse " + config_path + ": " + std::string(e.what())); }

    AttackerConfig config;
    try {
        config.blockchainFilePath = cfg_json.at("blockchainFilePath").get<std::string>();
        config.attackTarget = cfg_json.value("attackTarget", "last");
        config.useMultithreading = cfg_json.value("useMultithreading", true);
        config.numThreads = cfg_json.value("numThreads", 0);
        if (config.numThreads <= 0) {
            config.numThreads = std::thread::hardware_concurrency();
            if (config.numThreads == 0) config.numThreads = 1;
        }
        config.logFilePath = cfg_json.value("logFilePath", "bin/attack_log.txt");
        config.resultsFilePath = cfg_json.value("resultsFilePath", "bin/attack_results.txt");
        config.nTimesResultsFilePath = cfg_json.value("nTimesResultsFilePath", "bin/");
        config.attackEntropyValue = cfg_json.value("attackEntropyValue", "");
        config.useOriginalEntropy = cfg_json.value("useOriginalEntropy", false);
        config.stop_on_first_match = cfg_json.value("stop_on_first_match", true);
        config.attackMode = cfg_json.value("attackMode", 0);
        config.numberOfAttacks = cfg_json.value("numberOfAttacks", 1);

        config.dataLength = cfg_json.value("dataLength", 0);
        config.charSet = cfg_json.value("charSet", "");
        config.addressCharSet = cfg_json.value("addressCharSet", "");
        config.amountCharSet = cfg_json.value("amountCharSet", "");
        config.amountLength = cfg_json.value("amountLength", 0);
        config.senderAddressLength = cfg_json.value("senderAddressLength", 0);
        config.receiverAddressLength = cfg_json.value("receiverAddressLength", 0);
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error("FATAL ERROR: Invalid or missing field in " + config_path + ": " + std::string(e.what()));
    }
    return config;
}