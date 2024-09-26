#include "sys.h"


sys::sys() {
    #ifdef _WIN32
        if(std::system("choco --version > nul 2>&1") != 0) {
            if(std::system("powershell -Command \"Set-ExecutionPolicy Bypass -Scope Process\""))
                std::system("powershell -Command \"Set-ExecutionPolicy AllSigned\"")

            int install_result = std::system("powershell -Command \"Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))\"");
            if(install_result != 0) {
                throw std::runtime_error("Failed to install Chocolatey");
            }
        }
        packageManagerName = "choco";
    #elif __APPLE__
        int result = std::system("brew --version > /dev/null 2>&1");
        if(std::system("brew --version > /dev/null 2>&1") != 0) {
            int install_result = std::system("/bin/bash -c \"$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\"");

            if (install_result != 0) {
                throw std::runtime_error("Failed to install Homebrew");
            } 
        } 
        packageManagerName = "brew";
    #elif __MACH__
        int result = std::system("brew --version > /dev/null 2>&1");
        if(std::system("brew --version > /dev/null 2>&1") != 0) {
            int install_result = std::system("/bin/bash -c \"$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\"");

            if (install_result != 0) {
                throw std::runtime_error("Failed to install Homebrew");
            } 
        } 
        packageManagerName = "brew";
    #elif __linux__
        std::set<std::string> dnfUsers = {"fedora"};
        struct utsname sysinfo;
        if(uname(&sysinfo) != 0) {
            throw std::runtime_error("Failed to get system info");
        }
        std::string sysname = "fedora";
        if(dnfUsers.find(sysname) != dnfUsers.end()) {
            packageManagerName = "dnf";
        } else {
            throw std::runtime_error("Unknown package manager");
        }
    #else
        throw std::runtime_error("Unknown OS");
    #endif
}

sys::~sys() {
}

int sys::install(std::string package) {
    return this->install(std::vector<std::string>{package});
}

int sys::install(std::vector<std::string> packages) {
    std::string command = "";
    #ifdef __linux__
        command += "sudo ";
    #endif
    command += this->packageManagerName;
    command += " install";
    for(std::string package : packages) {
        command += " ";
        command += package;
    }
    
    return std::system(command.c_str());
}

int sys::uninstall(std::string package) {
    return this->uninstall(std::vector<std::string>{package});
}

int sys::uninstall(std::vector<std::string> packages) {
    std::string command = "";
    #ifdef __linux__
        command += "sudo ";
    #endif
    command += this->packageManagerName;
    command += " remove";
    for(std::string package : packages) {
        command += " ";
        command += package;
    }
    
    return std::system(command.c_str());
}

int sys::update(std::string package) {
    return this->update(std::vector<std::string>{package});
}

int sys::update(std::vector<std::string> packages) {
    std::string command = "";
    #ifdef __linux__
        command += "sudo ";
    #endif
    command += this->packageManagerName;
    if(this->packageManagerName == "brew" || this->packageManagerName == "choco") {
        command += " upgrade ";
    } else {
        if(this->packageManagerName == "dnf") {
            command += " upgrade";
        }
    }
    for(std::string package : packages) {
        command += " ";
        command += package;
    }
    
    return std::system(command.c_str());
}

int sys::update() {
    return this->update(std::vector<std::string>{});
}

int sys::list() {
    std::string command = "";
    #ifdef __linux__
        command += "sudo ";
    #endif
    
    if(this->packageManagerName == "choco") {
        command += "list --local-only";
    } else {
        command += this->packageManagerName;
        command += " list";
    }
    return std::system(command.c_str());
}

int sys::search(std::string package) {
    std::string command = "";
    #ifdef __linux__
        command += "sudo ";
    #endif
    command += this->packageManagerName;
    command += " search ";
    command += package;
    return std::system(command.c_str());
}

int sys::info(std::string package) {
    std::string command = "";
    #ifdef __linux__
        command += "sudo ";
    #endif
    command += this->packageManagerName;
    command += " info ";
    command += package;
    return std::system(command.c_str());
}

extern "C" PackageManager* create() {
    return new sys();
}

extern "C" void destroy(PackageManager * plugin) {
    delete plugin;
}