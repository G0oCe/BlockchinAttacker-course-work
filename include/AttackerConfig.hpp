#ifndef ATTACKER_CONFIG_HPP
#define ATTACKER_CONFIG_HPP

#include <string>
#include <vector>

// Структура конфигурации
struct AttackerConfig {
    std::string blockchainFilePath;
    std::string attackTarget;
    bool useMultithreading;
    int numThreads;
    std::string logFilePath;
    std::string resultsFilePath;
    std::string nTimesResultsFilePath;
    std::string attackEntropyValue;
    bool useOriginalEntropy = false;
    bool stop_on_first_match = true;
    int attackMode;
    int numberOfAttacks = 1;

    // Permutation Mode Settings
    int dataLength;
    std::string charSet;

    // Transaction Mode Settings
    std::string addressCharSet;
    std::string amountCharSet;
    int amountLength;
    int senderAddressLength;
    int receiverAddressLength;
};

// Функция загрузки конфигурации
AttackerConfig load_attacker_config(const std::string& config_path);

#endif // ATTACKER_CONFIG_HPP