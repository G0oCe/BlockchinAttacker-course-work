#include "Globals.hpp"

std::atomic<bool> g_solution_found{false};
std::atomic<unsigned long long> g_permutations_checked_total{0};

std::mutex g_result_mutex;
std::string g_found_data_string = "";
std::string g_found_hash = "";
std::string g_found_original_hash = "";
int g_found_block_index = -1;