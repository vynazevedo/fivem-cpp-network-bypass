#include "core/trace.hpp"
#include "core/network.hpp"
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <functional>

void clearScreen();
void displayLogo();
void displayMenu(const std::string& fivemPath, const std::string& launchBuild);
void handleMenuChoice(int choice);
bool initializeSystem();

std::unique_ptr<network> g_network;
std::unique_ptr<trace> g_trace;

struct MenuOption {
    std::string description;
    std::function<void()> action;
};

std::vector<MenuOption> menuOptions;

void initializeMenuOptions() {
    menuOptions = {
        {"Clean Traces", []() { g_trace->setup(); }},
        {"Enable Network Bypass", []() { g_network->setup(); }},
        {"Disable Network Bypass", []() { g_network->destroy(); }},
        {"Reset FiveM Path", []() { g_trace->destroy(); }},
        {"Edit Launch Options", []() { g_trace->set_launch_build(); }}
    };
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void displayLogo() {
    std::cout << R"(
    ███████╗██╗██╗   ██╗███████╗███╗   ███╗    ████████╗ ██████╗  ██████╗ ██╗     
    ██╔════╝██║██║   ██║██╔════╝████╗ ████║    ╚══██╔══╝██╔═══██╗██╔═══██╗██║     
    █████╗  ██║██║   ██║█████╗  ██╔████╔██║       ██║   ██║   ██║██║   ██║██║     
    ██╔══╝  ██║╚██╗ ██╔╝██╔══╝  ██║╚██╔╝██║       ██║   ██║   ██║██║   ██║██║     
    ██║     ██║ ╚████╔╝ ███████╗██║ ╚═╝ ██║       ██║   ╚██████╔╝╚██████╔╝███████╗
    ╚═╝     ╚═╝  ╚═══╝  ╚══════╝╚═╝     ╚═╝       ╚═╝    ╚═════╝  ╚═════╝ ╚══════╝
                        FiveM Network Management Tool v1.0 - vynazevedo
    )" << '\n';
}

void displayMenu(const std::string& fivemPath, const std::string& launchBuild) {
    std::cout << "\n  [System Information]" << std::endl;
    std::cout << "  ═══════════════════" << std::endl;
    std::cout << "  Selected Folder: " << (!fivemPath.empty() ? fivemPath : "Not Set") << std::endl;
    std::cout << "  Launch Build: " << (!launchBuild.empty() ? launchBuild : "Not Set") << std::endl;

    std::cout << "\n  [Available Options]" << std::endl;
    std::cout << "  ═══════════════════" << std::endl;
    for (size_t i = 0; i < menuOptions.size(); i++) {
        std::cout << "  [" << (i + 1) << "]  :  " << menuOptions[i].description << std::endl;
    }
    std::cout << "  [0]  :  Exit Program" << std::endl;
}

bool initializeSystem() {
    g_network = std::make_unique<network>();
    g_trace = std::make_unique<trace>();

    g_trace->m_fivem_path = g_trace->set_folder("Select FiveM Folder which contains FiveM.exe");
    return !g_trace->m_fivem_path.empty();
}

void handleMenuChoice(int choice) {
    if (choice >= 1 && choice <= static_cast<int>(menuOptions.size())) {
        menuOptions[choice - 1].action();
    }
}

int main() {
    initializeMenuOptions();
    
    while (true) {
        clearScreen();
        displayLogo();

        if (!initializeSystem()) {
            std::cout << "\n  [Error] Invalid FiveM folder selected." << std::endl;
            std::cout << "\n  Press Enter to retry...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue;
        }

        while (true) {
            clearScreen();
            displayLogo();
            displayMenu(g_trace->m_fivem_path, g_trace->get_launch_build());

            int choice;
            std::cout << "\n  Enter your choice (0-" << menuOptions.size() << "): ";
            
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "\n  Invalid input. Press Enter to continue...";
                std::cin.get();
                continue;
            }

            if (choice == 0) {
                std::cout << "\n  Exiting program...\n";
                return 0;
            }

            handleMenuChoice(choice);
            
            std::cout << "\n  Operation completed. Press Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }
    }
}