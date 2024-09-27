#ifndef NXPM_MANAGER_H
#define NXPM_MANAGER_H

#include <string>
#include <vector>
#include <argparse.hpp>
#include <filesystem>
#include "DynamicLibraryLoader.h"
#include "Downloader.h"
#include "sqlite3.h"
#include "PackageManager.h"
#include "sys.h"

class NxpmManager {
    private:
        int argc;
        char **argv;

        argparse::ArgumentParser * program;
        argparse::ArgumentParser * installer;
        argparse::ArgumentParser * uninstaller;
        argparse::ArgumentParser * updater;
        argparse::ArgumentParser * searcher;
        argparse::ArgumentParser * lister;
        argparse::ArgumentParser * finder;

        Downloader downloader;
        sqlite3 * db;
        sqlite3_stmt * stmt;
        

        std::string homePath;
        std::string nexisPath;
        std::string pmPath;
        std::string dbPath;
        std::string pkgPath;
        std::string nxdbPath;
        
        
        std::string target;
        std::string command;
        std::vector<std::string> packages;
        
        PackageManager * library;
    
    public:
        NxpmManager(int argc, char **argv);
        ~NxpmManager();
        
        int parseArguments();
        int checkAndInstallTools();
        int checkAndCreatePaths();
        int checkAndDownloadNxpmDb();
        int loadPackageManager();
        int runCommand();
};

#endif