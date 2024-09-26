#include "Downloader.h"

Downloader::Downloader() {
}

Downloader::~Downloader() {
}

int Downloader::downloadGit(std::string gitRepo, std::string path) {
    if(system("git --version > /dev/null") != 0) {
        std::cout << "Git is not installed" << std::endl;
        return 1;
    }
    std::string command = "git clone https://github.com/" + gitRepo + ".git " + path;
    return system(command.c_str());
}
