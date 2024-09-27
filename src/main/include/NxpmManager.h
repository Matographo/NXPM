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


/**
 * Class to manage the whole program
 */
class NxpmManager {
    private:
        // Program arguments
        int argc;
        char **argv;

        // Argument parsers
        argparse::ArgumentParser * program;
        argparse::ArgumentParser * installer;
        argparse::ArgumentParser * uninstaller;
        argparse::ArgumentParser * updater;
        argparse::ArgumentParser * searcher;
        argparse::ArgumentParser * lister;
        argparse::ArgumentParser * finder;


        // Tools
        Downloader downloader;
        sqlite3 * db;
        sqlite3_stmt * stmt;
        

        // Paths
        std::string homePath;
        std::string nexisPath;
        std::string pmPath;
        std::string dbPath;
        std::string pkgPath;
        std::string nxdbPath;
        
        
        // Command arguments
        std::string target;
        std::string command;
        std::vector<std::string> packages;
        
        // Package manager
        PackageManager * library;
        
        
        // Functions
        int createNewVersion(std::string version, bool isHash, std::string packageName, std::string pathToPackage, std::string pathToPackageVersion);
    
    public:
        NxpmManager(int argc, char **argv);
        ~NxpmManager();
        
        /**
         * Parse the arguments given to the program
         */
        int parseArguments();
        
        /**
         * Check if the tools are installed and install them if not
         */
        int checkAndInstallTools();
        
        /**
         * Check if the paths exist and create them if not
         */
        int checkAndCreatePaths();
        
        /**
         * Check if the Nxpm database exists and download it if not
         */
        int checkAndDownloadNxpmDb();
        
        /**
         * Load the package manager
         */
        int loadPackageManager();
        
        /**
         * Run the command given by the user with the given packages and load the package manager
         */
        int runCommand();
};

#endif