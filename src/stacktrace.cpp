#include <signal.h>
#include <execinfo.h>
#include <fc/log/logger.hpp>
#include <iostream>

#define FC_STACKTRACE_SKIP_ENTRIES 3
#define FC_STACKTRACE_ENTRIES FC_STACKTRACE_SKIP_ENTRIES+50

namespace fc {
    void print_stacktrace() {
        void* trace[FC_STACKTRACE_ENTRIES];
        size_t size = backtrace(trace, FC_STACKTRACE_ENTRIES);

        std::cout << "--- STACKTRACE:" << std::endl;

        char** strs = backtrace_symbols(trace, size);
        if (strs == NULL) {
            std::cout << "unknown" << std::endl;
            return;
        }

        for (size_t i = FC_STACKTRACE_SKIP_ENTRIES; i < size; ++i) {
            size_t n = i - FC_STACKTRACE_SKIP_ENTRIES + 1;
            std::cout << "#" << n << ": " << strs[i] << std::endl;
        }

        std::cout << "--- DEMANGLED STACKTRACE IS BELOW:" << std::endl;

        for (size_t i = FC_STACKTRACE_SKIP_ENTRIES; i < size; ++i) {
            std::string exe;
            std::string str(strs[i]);
            size_t end = str.find('(');
            if (end != std::string::npos) {
                exe = str.substr(0, end);
            }

            std::stringstream cmd;
            cmd << "addr2line " << trace[i] << " -e " << exe << " --functions --demangle";
            if (system(cmd.str().c_str()) != EXIT_SUCCESS) {
                std::cout << "addr2line not working, cannot demangle stacktrace entry" << std::endl;
            }
        }

        free(strs);
    }

    namespace {
        void stacktrace_signal_handler(int signum, const char* signal_name) {
            ::signal(signum, SIG_DFL);
            elog("Fatal error: ${signal_name}", ("signal_name", signal_name));
            print_stacktrace();
            ::exit(EXIT_FAILURE);
        }

        void stacktrace_sigsegv_handler(int signum) {
            stacktrace_signal_handler(signum, "SIGSEGV");
        }

        void stacktrace_sigabrt_handler(int signum) {
            stacktrace_signal_handler(signum, "SIGABRT");
        }

        void stacktrace_sigfpe_handler(int signum) {
            stacktrace_signal_handler(signum, "SIGFPE");
        }

        void stacktrace_sigill_handler(int signum) {
            stacktrace_signal_handler(signum, "SIGILL");
        }
    }

    void install_stacktrace_crash_handler() {
        ::signal(SIGSEGV, &stacktrace_sigsegv_handler);
        ::signal(SIGABRT, &stacktrace_sigabrt_handler);
        ::signal(SIGFPE, &stacktrace_sigfpe_handler);
        ::signal(SIGILL, &stacktrace_sigill_handler);
    }
}
