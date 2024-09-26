#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <string>
#include <vector>

class PackageManager {
private:

public:
    virtual ~PackageManager() = default;
  
    virtual int install(std::string package) = 0;
    virtual int install(std::vector<std::string> packages) = 0;
    virtual int uninstall(std::string package) = 0;
    virtual int uninstall(std::vector<std::string> packages) = 0;
    virtual int update(std::string package) = 0;
    virtual int update(std::vector<std::string> packages) = 0;
    virtual int update() = 0;
    virtual int search(std::string package) = 0;
    virtual int list() = 0;
    virtual int info(std::string package) = 0;
};

#endif