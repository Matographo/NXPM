#ifndef sys_h
#define sys_h

#include <iostream>
#include <set>
#include <sys/utsname.h>
#include "PackageManager.h"
#ifdef _WIN32
    #include <windows.h>
#endif


class sys : public PackageManager {
    private:
        std::string packageManagerName;
    public:
        
        sys();
        virtual ~sys();
        
        int install(std::string package) override;
        int install(std::vector<std::string> packages) override;
        int uninstall(std::string package) override;
        int uninstall(std::vector<std::string> packages) override;
        int update(std::string package) override;
        int update(std::vector<std::string> packages) override;
        int update() override;
        int search(std::string package) override;
        int list() override;
        int info(std::string package) override;
};


#endif