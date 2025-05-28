cmake_minimum_required(VERSION 3.10)
project(BlockchainAttacker LANGUAGES CXX)

# --- Compiler Settings ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -pedantic)

# --- Source and Header Files ---
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

# --- Include Directories ---
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- Dependencies ---
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

find_path(CRYPTOPP_INCLUDE_DIR cryptlib.h PATH_SUFFIXES cryptopp)
find_library(CRYPTOPP_LIBRARY NAMES cryptopp libcryptopp)

if (CRYPTOPP_INCLUDE_DIR)
    include_directories(${CRYPTOPP_INCLUDE_DIR})
endif()

# --- Build Target ---
add_executable(blockchain_attacker ${SRC_FILES} ${HEADER_FILES})

# --- Output Directory ---
set_target_properties(blockchain_attacker PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# --- Linking ---
target_link_libraries(blockchain_attacker PRIVATE
        Threads::Threads
        OpenSSL::Crypto
)

if (CRYPTOPP_LIBRARY)
    target_link_libraries(blockchain_attacker PRIVATE ${CRYPTOPP_LIBRARY})
endif()

# --- User Instructions ---
message(STATUS "BlockchainAttacker project configured.")
message(STATUS "To build:")
message(STATUS "  1. Ensure you are in the build directory (e.g., 'mkdir build && cd build' from project root).")
message(STATUS "  2. Run CMake: 'cmake ..'")
message(STATUS "     For an optimized (Release) build: 'cmake .. -DCMAKE_BUILD_TYPE=Release'")
message(STATUS "     For a debug build: 'cmake .. -DCMAKE_BUILD_TYPE=Debug'")
message(STATUS "  3. Compile: 'make' (or your build tool like 'ninja', or 'cmake --build .')")
message(STATUS "The executable will be in: ${CMAKE_BINARY_DIR}/bin/blockchain_attacker")