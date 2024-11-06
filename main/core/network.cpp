#include "network.hpp"
#include "trace.hpp"
#include <filesystem>
#include <format>
#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>

namespace fs = std::filesystem;

class NetworkManager {
private:
    const std::vector<std::string> GTA_VERSIONS = {
        "b2845_", "b2802_", "b2699_", "b2612_", "b2545_", 
        "b2372_", "b2189_", "b2060_", ""
    };

    const std::vector<std::string> PROCESS_TYPES = {
        "GTAProcess", "SteamChild", "ROSService", 
        "LauncherService", "ChromeBrowser"
    };

    bool executeCommand(const std::string& cmd) {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        char cmdBuffer[MAX_PATH];
        strncpy_s(cmdBuffer, cmd.c_str(), cmd.length() + 1);

        if (!CreateProcessA(NULL, cmdBuffer, NULL, NULL, FALSE, 
            CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            return false;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return exitCode == 0;
    }

    void setFirewallRule(const std::string& process, bool block) {
        if (process.empty() || !fs::exists(process)) {
            std::cerr << "Invalid process path: " << process << std::endl;
            return;
        }

        const auto action = block ? "block" : "allow";
        const auto operation = block ? "add" : "delete";

        for (const auto& direction : {"in", "out"}) {
            std::string cmd = std::format(
                "netsh advfirewall firewall {} rule name=\"{}\" dir={} program=\"{}\" action={} enable=yes",
                operation, fs::path(process).filename().string(), direction, process, action);

            if (!executeCommand(cmd)) {
                std::cerr << "Failed to " << operation << " " << direction << "bound rule for: " << process << std::endl;
            }
        }
    }

    void displayProgress(const std::string& operation, int current, int total) {
        const int barWidth = 50;
        float progress = (float)current / total;
        int pos = barWidth * progress;

        std::cout << "\r" << operation << ": [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << "%" << std::flush;
    }

public:
    void setup(const std::string& fivemPath) {
        try {
            if (!fs::exists(fivemPath)) {
                throw std::runtime_error("FiveM path does not exist");
            }

            std::cout << "\n[+] Initializing network protection...\n" << std::endl;

            const fs::path basePath = fivemPath;
            const fs::path subprocessPath = basePath / "FiveM.app" / "data" / "cache" / "subprocess";
            
            int totalOperations = GTA_VERSIONS.size() * PROCESS_TYPES.size() + 1;
            int currentOperation = 0;

            setFirewallRule((basePath / "FiveM.exe").string(), true);
            displayProgress("Setting up rules", ++currentOperation, totalOperations);

            for (const auto& version : GTA_VERSIONS) {
                for (const auto& processType : PROCESS_TYPES) {
                    auto processPath = subprocessPath / std::format("FiveM_{}{}.exe", version, processType);
                    if (fs::exists(processPath)) {
                        setFirewallRule(processPath.string(), true);
                    }
                    displayProgress("Setting up rules", ++currentOperation, totalOperations);
                }
            }

            std::cout << "\n\n[+] Network protection enabled successfully!" << std::endl;
            std::cout << "[+] Total rules created: " << currentOperation << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "\n[-] Error during setup: " << e.what() << std::endl;
        }
    }

    void destroy(const std::string& fivemPath) {
        try {
            if (!fs::exists(fivemPath)) {
                throw std::runtime_error("FiveM path does not exist");
            }

            std::cout << "\n[+] Removing network protection...\n" << std::endl;

            const fs::path basePath = fivemPath;
            const fs::path subprocessPath = basePath / "FiveM.app" / "data" / "cache" / "subprocess";
            
            int totalOperations = GTA_VERSIONS.size() * PROCESS_TYPES.size() + 1;
            int currentOperation = 0;

            setFirewallRule((basePath / "FiveM.exe").string(), false);
            displayProgress("Removing rules", ++currentOperation, totalOperations);

            for (const auto& version : GTA_VERSIONS) {
                for (const auto& processType : PROCESS_TYPES) {
                    auto processPath = subprocessPath / std::format("FiveM_{}{}.exe", version, processType);
                    if (fs::exists(processPath)) {
                        setFirewallRule(processPath.string(), false);
                    }
                    displayProgress("Removing rules", ++currentOperation, totalOperations);
                }
            }

            std::cout << "\n\n[+] Network protection removed successfully!" << std::endl;
            std::cout << "[+] Total rules removed: " << currentOperation << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "\n[-] Error during cleanup: " << e.what() << std::endl;
        }
    }
};

void network::block_connection(const std::string& process) {
    NetworkManager mgr;
    mgr.setup(process);
}

void network::unblock_connection(const std::string& process) {
    NetworkManager mgr;
    mgr.destroy(process);
}

void network::setup() {
    NetworkManager mgr;
    mgr.setup(g_trace->m_fivem_path);
}

void network::destroy() {
    NetworkManager mgr;
    mgr.destroy(g_trace->m_fivem_path);
}