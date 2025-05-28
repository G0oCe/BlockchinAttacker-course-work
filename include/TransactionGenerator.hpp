#ifndef TRANSACTION_GENERATOR_HPP
#define TRANSACTION_GENERATOR_HPP

#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm> // Для std::reverse
#include <limits>    // Для numeric_limits
#include <iostream>  // Для вывода
#include <sstream>   // Для форматирования
#include <mutex>     // <<< Для потокобезопасности
#include <chrono>    // <<< Для временных меток
#include <iomanip>   // <<< Для временных меток
#include "console_colors.hpp" // <<< Для цветного вывода

// --- ЛОГИРОВАНИЕ ---
// Раскомментируйте следующую строку, чтобы включить логирование
 #define ENABLE_TG_LOGGING

namespace {
    std::mutex tg_log_mutex;

    // Функция для получения временной метки в стиле main_attacker
    std::string get_tg_timestamp() {
        auto now_sys = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now_sys);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_sys.time_since_epoch()) % 1000;

        std::stringstream ss_timestamp;
#if defined(_MSC_VER)
        struct tm buf;
            localtime_s(&buf, &in_time_t);
            // Используем полный формат, как в main_attacker
            ss_timestamp << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
#else
        // Используем полный формат, как в main_attacker
        ss_timestamp << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
#endif
        ss_timestamp << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss_timestamp.str();
    }
}

#ifdef ENABLE_TG_LOGGING
// Макрос для логирования, имитирующий safe_log
    #define TG_LOG(message, color) \
        do { \
            std::lock_guard<std::mutex> guard(tg_log_mutex); \
            std::ostringstream oss; \
            oss << message; \
            std::cout << color << "[TG:" << get_tg_timestamp() << "] " \
                      << oss.str() << Color::RESET << std::endl; \
        } while (0)
    // Версия без цвета (использует CYAN по умолчанию)
    #define TG_LOG_DEFAULT(message) TG_LOG(message, Color::CYAN)

#else
#define TG_LOG(message, color) do {} while (0)
#define TG_LOG_DEFAULT(message) do {} while (0)
#endif
// --- КОНЕЦ ЛОГИРОВАНИЯ ---


namespace { // Вспомогательная функция в анонимном пространстве имен
    // Безопасная функция возведения в степень с проверкой на переполнение
    unsigned long long safe_pow(unsigned long long base, int exp) {
        unsigned long long res = 1;
        if (exp < 0) throw std::runtime_error("Exponent cannot be negative.");
        if (exp == 0) return 1;
        if (base == 0) return 0;
        if (base == 1) return 1;

        for (int i = 0; i < exp; ++i) {
            if (std::numeric_limits<unsigned long long>::max() / base < res) {
                throw std::overflow_error("Power calculation resulted in overflow.");
            }
            res *= base;
        }
        return res;
    }
}

class TransactionGenerator {
private:
    std::string addressCharSet;
    std::string amountCharSet;
    int senderAddressLength;
    int receiverAddressLength;
    int amountLength;

    unsigned long long n_sender;
    unsigned long long n_receiver;
    unsigned long long n_amount;
    unsigned long long total_permutations;

    // Генерирует 'index'-ую строку (0-based) используя заданный charset и length.
    std::string getStringByIndex(const std::string& charset, int length, unsigned long long index) const {
        if (charset.empty()) {
            throw std::runtime_error("Charset cannot be empty for getStringByIndex.");
        }
        if (length <= 0) {
            return "";
        }

        unsigned long long base = charset.size();
        std::string result = "";
        result.reserve(length);
        unsigned long long temp_index = index;

        for (int i = 0; i < length; ++i) {
            result += charset[temp_index % base];
            temp_index /= base;
        }

        while (result.length() < length) {
            result += charset[0];
        }

        std::reverse(result.begin(), result.end());
        return result;
    }


public:
    TransactionGenerator(const std::string& addrCS, const std::string& amtCS, int senderL, int receiverL, int amtL)
            : addressCharSet(addrCS), amountCharSet(amtCS),
              senderAddressLength(senderL), receiverAddressLength(receiverL), amountLength(amtL) {

        TG_LOG_DEFAULT("Вызов конструктора TransactionGenerator.");
        TG_LOG_DEFAULT("  Address CharSet: '" << addrCS << "'");
        TG_LOG_DEFAULT("  Amount CharSet:  '" << amtCS << "'");
        TG_LOG_DEFAULT("  Длины: Sender=" << senderL << ", Receiver=" << receiverL << ", Amount=" << amtL);


        if (addressCharSet.empty() || amountCharSet.empty() || senderAddressLength <= 0 || receiverAddressLength <= 0 || amountLength <= 0) {
            TG_LOG("  ОШИБКА: Неверные параметры.", Color::RED);
            throw std::runtime_error("Invalid parameters for TransactionGenerator: Empty charset or non-positive length.");
        }

        try {
            n_sender = safe_pow(addressCharSet.size(), senderAddressLength);
            n_receiver = safe_pow(addressCharSet.size(), receiverAddressLength);
            n_amount = safe_pow(amountCharSet.size(), amountLength);
            TG_LOG_DEFAULT("  Рассчитано: N_Sender=" << n_sender << ", N_Receiver=" << n_receiver << ", N_Amount=" << n_amount);
        } catch (const std::overflow_error& e) {
            TG_LOG("  ОШИБКА: Переполнение при возведении в степень: " << e.what(), Color::RED);
            throw std::runtime_error("Failed to calculate permutation counts due to overflow: " + std::string(e.what()));
        }

        unsigned long long temp_total = n_sender;
        if (n_receiver > 0 && std::numeric_limits<unsigned long long>::max() / n_receiver < temp_total) {
            TG_LOG("  ОШИБКА: Переполнение при расчете общего числа (sender * receiver).", Color::RED);
            throw std::runtime_error("Total permutations calculation resulted in overflow (sender * receiver).");
        }
        temp_total *= n_receiver;
        if (n_amount > 0 && std::numeric_limits<unsigned long long>::max() / n_amount < temp_total) {
            TG_LOG("  ОШИБКА: Переполнение при расчете общего числа (* amount).", Color::RED);
            throw std::runtime_error("Total permutations calculation resulted in overflow ( * amount).");
        }
        total_permutations = temp_total * n_amount;

        TG_LOG_DEFAULT("  Общее число комбинаций: " << total_permutations);


        if (total_permutations == 0) {
            TG_LOG("  ОШИБКА: Общее число комбинаций равно нулю.", Color::RED);
            throw std::runtime_error("TransactionGenerator resulted in zero permutations (check parameters).");
        }
        TG_LOG("Конструктор TransactionGenerator успешно завершен.", Color::GREEN);
    }

    unsigned long long getTotalPermutations() const {
        return total_permutations;
    }

    std::string getPermutation(unsigned long long k) const {
        // Логирование для getPermutation может быть слишком частым,
        // поэтому оставим его более простым или закомментированным по умолчанию.
        // Если нужно, можно добавить TG_LOG_DEFAULT("Вызов getPermutation(" << k << ").");
        if (k >= total_permutations) {
            TG_LOG("  ОШИБКА: Индекс " << k << " вне диапазона. Всего: " << total_permutations, Color::RED);
            throw std::out_of_range("Index k (" + std::to_string(k) + ") is out of range. Total permutations: " + std::to_string(total_permutations));
        }

        unsigned long long index_amount = k % n_amount;
        unsigned long long k_temp = k / n_amount;
        unsigned long long index_receiver = k_temp % n_receiver;
        unsigned long long index_sender = k_temp / n_receiver;

        std::string sender = getStringByIndex(addressCharSet, senderAddressLength, index_sender);
        std::string receiver = getStringByIndex(addressCharSet, receiverAddressLength, index_receiver);
        std::string amount = getStringByIndex(amountCharSet, amountLength, index_amount);

        std::string result = sender + receiver + amount;
        // TG_LOG_DEFAULT("  Сгенерировано: '" << result << "'"); // Можно раскомментировать для детальной отладки
        return result;
    }
};

#endif // TRANSACTION_GENERATOR_HPP