#ifndef CPP_H
#define CPP_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "PackageManager.h"
#include "sqlite3.h"
#include "Downloader.h"

class Cpp : public PackageManager {
private:
    sqlite3 *db;
    sqlite3_stmt *stmt;

    int createNewVersion(std::string version, bool isHash, std::string packageName, std::string pathToPackage, std::string pathToPackageVersion);
public:
    Cpp();
    virtual ~Cpp();
    
    virtual int install(std::string package) override;
    virtual int install(std::vector<std::string> packages) override;
    virtual int uninstall(std::string package) override;
    virtual int uninstall(std::vector<std::string> packages) override;
    virtual int update(std::string package) override;
    virtual int update(std::vector<std::string> packages) override;
    virtual int update() override;
    virtual int search(std::string package) override;
    virtual int list() override;
    virtual int info(std::string package) override;
};

#endif