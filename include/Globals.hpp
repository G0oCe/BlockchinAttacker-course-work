#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <atomic>
#include <string>
#include <mutex>

extern std::atomic<bool> g_solution_found;
extern std::atomic<unsigned long long> g_permutations_checked_total;

extern std::mutex g_result_mutex;
extern std::string g_found_data_string;
extern std::string g_found_hash;
extern std::string g_found_original_hash;
extern int g_found_block_index;

#endif // GLOBALS_HPP