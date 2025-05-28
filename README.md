# BlockchainAttacker-course-work

## Description

BlockchainAttacker is a C++ project designed to simulate attacks on a simplified blockchain. It reads blockchain data from a JSON file and attempts to find data that, when hashed within a block structure, matches the block's original hash. The project supports configurable attack parameters, including different attack modes (permutation or transaction data generation), multithreading, and logging.

## Features

* **Blockchain Parsing:** Parses blockchain data from a JSON file.
* **Configurable Attacks:** Attack parameters are managed through a `config_attacker.json` file.
* **Attack Modes:**
    * **Permutation Mode (0):** Generates permutations of a character set of a defined length to find matching data.
    * **Transaction Mode (1):** Generates mock transaction data (sender address, receiver address, amount) based on defined character sets and lengths.
* **Multithreading:** Supports using multiple threads to speed up the attack process.
* **Hashing:** Uses SHA256 for hashing and includes a Merkle root calculation function.
* **Logging:** Outputs attack progress to the console and to log files.
* **Multiple Attack Runs:** Can run the attack simulation multiple times and provides a summary.

## Dependencies

The project requires the following dependencies:
* CMake (version 3.10 or higher)
* A C++17 compliant compiler
* OpenSSL (Crypto library)
* Threads
* Crypto++ (libcryptopp) - The build system will try to find it.
* nlohmann/json library (included in the `include/nlohmann` directory).

## Build Instructions

To build the project, follow these steps:
1.  Ensure you are in the project root directory.
2.  Create a build directory and navigate into it:
    ```bash
    mkdir build
    cd build
    ```
3.  Run CMake to configure the project. Replace `..` with the path to the project root if you are in a different build directory structure.
    * For a standard build:
        ```bash
        cmake ..
        ```
    * For an optimized (Release) build:
        ```bash
        cmake .. -DCMAKE_BUILD_TYPE=Release
        ```
    * For a debug build:
        ```bash
        cmake .. -DCMAKE_BUILD_TYPE=Debug
        ```
4.  Compile the project:
    ```bash
    make
    ```
    (Or use your preferred build tool like `ninja`, or `cmake --build .`)

The executable will be located in the `build/bin/` directory (e.g., `build/bin/blockchain_attacker`).

## Configuration

The attacker's behavior is configured using a JSON file, typically `config_attacker.json`. The default path is `../../config_attacker.json` relative to where the executable is run (likely from `build/bin`, so it would look for `config_attacker.json` in the project root).

Key configuration options in `config_attacker.json`:
* `blockchainFilePath`: Path to the JSON file containing the blockchain data.
* `attackTarget`: Specifies which block(s) to attack.
    * `"last"`: Attacks only the last block in the chain.
    * `"all"`: Attempts to attack all blocks in the chain sequentially.
* `useMultithreading`: `true` to enable multithreading, `false` otherwise.
* `numThreads`: Number of threads to use if multithreading is enabled. If set to 0 or less, it defaults to `std::thread::hardware_concurrency()`.
* `logFilePath`: Path for the general log file.
* `resultsFilePath`: Path for the detailed attack results log.
* `nTimesResultsFilePath`: Base path and prefix for the summary file when `numberOfAttacks` > 1. The actual filename will have `_<numberOfAttacks>_times.txt` and a potential counter like `(1)` appended to avoid overwriting.
* `attackEntropyValue`: String to use as the entropy part of the block data if `useOriginalEntropy` is false.
* `useOriginalEntropy`: If `true`, uses the original entropy string from the block data (if present, typically `data[1]`). If `false`, uses `attackEntropyValue`.
* `stop_on_first_match`: If `true`, the attack on a block (or the entire run if `attackTarget` is `all`) stops as soon as a solution is found.
* `numberOfAttacks`: How many times to run the entire attack simulation. Useful for performance measurement.
* `attackMode`:
    * `0`: Permutation mode.
    * `1`: Transaction mode.
* **Permutation Mode Settings** (`attackMode: 0`):
    * `dataLength`: The length of the string to generate by permutation.
    * `charSet`: The set of characters to use for generating permutations.
* **Transaction Mode Settings** (`attackMode: 1`):
    * `addressCharSet`: Character set for sender and receiver addresses.
    * `amountCharSet`: Character set for transaction amounts.
    * `amountLength`: Length of the transaction amount string.
    * `senderAddressLength`: Length of the sender's address string.
    * `receiverAddressLength`: Length of the receiver's address string.
