#pragma once
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <io.h> // For _isatty
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h> // For isatty
#define ISATTY isatty
#define FILENO fileno
#endif


namespace Color {
    // Function to check if the stream is a TTY (terminal)
    inline bool isTerminal(FILE* stream = stdout) {
        return ISATTY(FILENO(stream));
    }

    // Function to enable virtual terminal processing on Windows for ANSI codes
#ifdef _WIN32
    inline void enableVirtualTerminalProcessing() {
        static bool enabled = false;
        if (!enabled) {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut != INVALID_HANDLE_VALUE) {
                DWORD dwMode = 0;
                if (GetConsoleMode(hOut, &dwMode)) {
                    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                    SetConsoleMode(hOut, dwMode);
                }
            }
            enabled = true; // Attempt only once
        }
    }
#else
    inline void enableVirtualTerminalProcessing() { /* No-op on non-Windows */ }
#endif


    // Define colors - empty if not a TTY
    const std::string RESET   = isTerminal() ? "\033[0m"  : "";
    const std::string BLACK   = isTerminal() ? "\033[30m" : "";
    const std::string RED     = isTerminal() ? "\033[31m" : "";
    const std::string GREEN   = isTerminal() ? "\033[32m" : "";
    const std::string YELLOW  = isTerminal() ? "\033[33m" : "";
    const std::string BLUE    = isTerminal() ? "\033[34m" : "";
    const std::string MAGENTA = isTerminal() ? "\033[35m" : "";
    const std::string CYAN    = isTerminal() ? "\033[36m" : "";
    const std::string WHITE   = isTerminal() ? "\033[37m" : "";
    const std::string BOLD    = isTerminal() ? "\033[1m"  : "";
}