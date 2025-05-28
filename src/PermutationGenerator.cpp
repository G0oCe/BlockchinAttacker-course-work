#include "PermutationGenerator.hpp"
#include <stdexcept> // For std::overflow_error, std::runtime_error
#include <limits>    // For numeric_limits
#include <algorithm> // For std::reverse


// Безопасная функция возведения в степень
unsigned long long PermutationGenerator::safe_pow(unsigned long long base, int exp) {
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

void PermutationGenerator::calculateTotalPermutations() {
    if (permLength == 0) {
        totalPermutations = 1; // Одна перестановка: пустая строка
        return;
    }
    if (characters.empty()) {
        totalPermutations = 0; // Нет перестановок
        return;
    }
    try {
        totalPermutations = safe_pow(characters.length(), permLength);
    } catch (const std::overflow_error&) {
        totalPermutations = 0; // Или можно пробросить исключение дальше
        throw; // Пробрасываем ошибку переполнения
    }
}

PermutationGenerator::PermutationGenerator(const std::string& charSet, int length)
        : characters(charSet), permLength(length) {
    if (permLength < 0) {
        throw std::invalid_argument("Length cannot be negative.");
    }
    calculateTotalPermutations();
}

unsigned long long PermutationGenerator::getTotalPermutations() const {
    return totalPermutations;
}

std::string PermutationGenerator::getStringByIndex(unsigned long long index) const {
    if (permLength == 0) return "";
    if (characters.empty()) throw std::runtime_error("Charset cannot be empty for getStringByIndex.");

    unsigned long long base = characters.size();
    std::string result = "";
    result.reserve(permLength);
    unsigned long long temp_index = index;

    for (int i = 0; i < permLength; ++i) {
        result += characters[temp_index % base];
        temp_index /= base;
    }

    while (result.length() < permLength) {
        result += characters[0];
    }

    std::reverse(result.begin(), result.end());
    return result;
}

std::string PermutationGenerator::getPermutation(unsigned long long index) const {
    if (totalPermutations == 0 && permLength > 0) {
        throw std::runtime_error("Cannot get permutation: Zero total permutations.");
    }
    if (permLength == 0) {
        return ""; // Единственная перестановка для длины 0
    }
    if (index >= totalPermutations) {
        throw std::out_of_range("Index k is out of range.");
    }
    return getStringByIndex(index);
}