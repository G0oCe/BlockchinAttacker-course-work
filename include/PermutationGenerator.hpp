#ifndef PERMUTATION_GENERATOR_HPP
#define PERMUTATION_GENERATOR_HPP

#include <string>
#include <vector>
#include <stdexcept>

class PermutationGenerator {
private:
    std::string characters;
    int permLength;
    unsigned long long totalPermutations;

    void calculateTotalPermutations();
    unsigned long long safe_pow(unsigned long long base, int exp);
    std::string getStringByIndex(unsigned long long index) const;

public:
    PermutationGenerator(const std::string& charSet, int length);
    unsigned long long getTotalPermutations() const;
    std::string getPermutation(unsigned long long index) const;
};

#endif // PERMUTATION_GENERATOR_HPP