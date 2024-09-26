#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <iostream>
#include <string>

class Downloader {
public:
    Downloader();
    ~Downloader();
    
    int downloadGit(std::string gitRepo, std::string path);
};

#endif