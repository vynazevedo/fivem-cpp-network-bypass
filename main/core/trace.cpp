#include "trace.hpp"
#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <format>

namespace fs = std::filesystem;

class TraceManager {
private:
    const std::vector<std::string> SUPPORTED_BUILDS = {
        "2845", "2802", "2699", "2612", "2545", 
        "2372", "2189", "2060", "1604"
    };

    const std::vector<std::pair<std::string, std::string>> CLEANUP_PATHS = {
        {"%LOCALAPPDATA%", "DigitalEntitlements"},
        {"%APPDATA%", "CitizenFX"},
        {"%LOCALAPPDATA%", "FiveM\\crashes"},
        {"%LOCALAPPDATA%", "FiveM\\logs"}
    };

    std::string expandEnvPath(const std::string& path) {
        char expanded[MAX_PATH];
        ExpandEnvironmentStringsA(path.c_str(), expanded, MAX_PATH);
        return std::string(expanded);
    }

    bool removeDirectory(const std::string& path) {
        try {
            if (fs::exists(path)) {
                const auto count = fs::remove_all(path);
                std::cout << "[+] Removed " << count << " files from: " << path << std::endl;
                return true;
            }
            return false;
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "[-] Error removing " << path << ": " << e.what() << std::endl;
            return false;
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
    void cleanupTraces() {
        std::cout << "\n[+] Starting system cleanup...\n" << std::endl;
        int totalPaths = CLEANUP_PATHS.size();
        int currentPath = 0;

        for (const auto& [env, path] : CLEANUP_PATHS) {
            std::string fullPath = expandEnvPath(env) + "\\" + path;
            displayProgress("Cleaning traces", ++currentPath, totalPaths);
            
            if (removeDirectory(fullPath)) {
                std::cout << "\n[+] Successfully cleaned: " << path << std::endl;
            } else {
                std::cout << "\n[!] Path not found: " << path << std::endl;
            }
        }

        std::cout << "\n[+] Cleanup completed!" << std::endl;
    }

    std::string selectFiveMFolder(const std::string& title, const std::string& savePath) {
        if (fs::exists(savePath)) {
            std::ifstream file(savePath);
            std::string savedPath;
            std::getline(file, savedPath);
            if (fs::exists(savedPath + "\\FiveM.exe")) {
                return savedPath;
            }
        }

        BROWSEINFOA bi = {0};
        char path[MAX_PATH] = {0};
        std::string defaultPath = expandEnvPath("%LOCALAPPDATA%\\FiveM");

        bi.lpszTitle = title.c_str();
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        bi.lpfn = [](HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData) -> int {
            if (uMsg == BFFM_INITIALIZED && pData) {
                SendMessageA(hwnd, BFFM_SETSELECTION, TRUE, pData);
            }
            return 0;
        };
        bi.lParam = (LPARAM)defaultPath.c_str();

        LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
        if (!pidl) {
            return "";
        }

        if (SHGetPathFromIDListA(pidl, path)) {
            CoTaskMemFree(pidl);
            std::string selectedPath = path;

            if (!fs::exists(selectedPath + "\\FiveM.exe")) {
                std::cerr << "[-] Error: FiveM.exe not found in selected directory!" << std::endl;
                return "";
            }

            std::ofstream file(savePath);
            file << selectedPath;

            return selectedPath;
        }

        CoTaskMemFree(pidl);
        return "";
    }

    void setBuildNumber(const std::string& fivemPath, const std::string& citizenPath) {
        system("cls");
        std::cout << "\n[+] Available Build Numbers:\n" << std::endl;

        for (size_t i = 0; i < SUPPORTED_BUILDS.size(); i++) {
            std::cout << std::format("  [{:2d}] : Build {}", i + 1, SUPPORTED_BUILDS[i]) << std::endl;
        }

        int choice;
        std::cout << "\nSelect build number (1-" << SUPPORTED_BUILDS.size() << "): ";
        std::cin >> choice;

        if (choice < 1 || choice > SUPPORTED_BUILDS.size()) {
            std::cerr << "[-] Invalid build number selected!" << std::endl;
            return;
        }

        std::string iniPath = fivemPath + citizenPath;
        if (!fs::exists(iniPath)) {
            std::ofstream createIni(iniPath);
            createIni.close();
        }

        if (WritePrivateProfileStringA("Game", "SavedBuildNumber", 
            SUPPORTED_BUILDS[choice - 1].c_str(), iniPath.c_str())) {
            std::cout << "[+] Successfully set build number to: " << SUPPORTED_BUILDS[choice - 1] << std::endl;
        } else {
            std::cerr << "[-] Failed to set build number!" << std::endl;
        }
    }

    std::string getBuildNumber(const std::string& fivemPath, const std::string& citizenPath) {
        std::string iniPath = fivemPath + citizenPath;
        if (!fs::exists(iniPath)) {
            return "Unknown";
        }

        char buildNumber[32] = {0};
        GetPrivateProfileStringA("Game", "SavedBuildNumber", "Unknown", 
            buildNumber, sizeof(buildNumber), iniPath.c_str());
        
        return std::string(buildNumber);
    }
};

void trace::setup() {
    TraceManager mgr;
    mgr.cleanupTraces();
}

void trace::destroy() {
    m_fivem_path.clear();
    if (fs::exists(m_save_path)) {
        fs::remove(m_save_path);
    }
}

std::string trace::set_folder(std::string title) {
    TraceManager mgr;
    return mgr.selectFiveMFolder(title, m_save_path);
}

void trace::set_launch_build() {
    TraceManager mgr;
    mgr.setBuildNumber(m_fivem_path, m_citizen_ini_path);
}

std::string trace::get_launch_build() {
    TraceManager mgr;
    return mgr.getBuildNumber(m_fivem_path, m_citizen_ini_path);
}