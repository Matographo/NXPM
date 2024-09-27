#include "NxpmManager.h"

NxpmManager::NxpmManager(int argc, char **argv) {
    this->argc = argc;
    this->argv = argv;

    this->homePath  = getenv("HOME");
    this->nexisPath = homePath  + "/.nxpm";
    this->pmPath    = nexisPath + "/package-managers";
    this->dbPath    = nexisPath + "/databases";
    this->pkgPath   = nexisPath + "/packages";
    this->nxdbPath  = dbPath    + "/nxpm";
    
    this->program     = &argparse::ArgumentParser("nxpm");
    this->installer   = &argparse::ArgumentParser("install");
    this->uninstaller = &argparse::ArgumentParser("uninstall");
    this->updater     = &argparse::ArgumentParser("update");
    this->searcher    = &argparse::ArgumentParser("search");
    this->lister      = &argparse::ArgumentParser("list");
    this->finder      = &argparse::ArgumentParser("find");
    
    this->installer->add_argument("package")
        .nargs(argparse::nargs_pattern::at_least_one)
        .help("The package to install");
    
    this->uninstaller->add_argument("package")
        .nargs(argparse::nargs_pattern::at_least_one)
        .help("The package to uninstall");

    this->updater->add_argument("package")
        .nargs(argparse::nargs_pattern::any)
        .help("The package to update");

    this->searcher->add_argument("package")
        .nargs(argparse::nargs_pattern::at_least_one)
        .help("The package to search for");
    
    this->lister->add_argument("package")
        .nargs(0)
        .help("All packages installed");
    
    this->finder->add_argument("package")
        .nargs(1)
        .help("The package to find");

    
    this->program->add_argument("target")
        .help("The target to run the command on");
    

    this->program->add_subparser(*this->installer);
    this->program->add_subparser(*this->uninstaller);
    this->program->add_subparser(*this->updater);
    this->program->add_subparser(*this->searcher);
    this->program->add_subparser(*this->lister);
    this->program->add_subparser(*this->finder);

}

NxpmManager::~NxpmManager() {
    delete program;
    delete library;
    sqlite3_close(this->db);
}

int NxpmManager::parseArguments() {
    try {
        this->program->parse_args(this->argc, this->argv);
    } catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        return 1;
    }
    
    this->target = this->program->get<std::string>("target");
    

    if(this->program->is_subcommand_used("install")) {
        this->command = "install";
        this->packages = this->installer->get<std::vector<std::string>>("package");
    } else if(program->is_subcommand_used("uninstall")) {
        this->command = "uninstall";
        this->packages = uninstaller->get<std::vector<std::string>>("package");
    } else if(program->is_subcommand_used("update")) {
        this->command = "update";
        this->packages = updater->get<std::vector<std::string>>("package");
    } else if(program->is_subcommand_used("search")) {
        this->command = "search";
        this->packages = searcher->get<std::vector<std::string>>("package");
    } else if(program->is_subcommand_used("list")) {
        this->command = "list";
    } else if(program->is_subcommand_used("find")) {
        this->command = "find";
        this->packages = finder->get<std::vector<std::string>>("package");
    }
    
    return 0;
}


int NxpmManager::checkAndCreatePaths() {
    bool mustDownload = false;
    if(!std::filesystem::exists(this->nexisPath)) {
        std::filesystem::create_directory(this->nexisPath);
        mustDownload = true;
    }
    
    if(!std::filesystem::exists(this->pmPath)) {
        std::filesystem::create_directory(this->pmPath);
        mustDownload = true;
    }

    if(!std::filesystem::exists(this->dbPath)) {
        std::filesystem::create_directory(this->dbPath);
        mustDownload = true;
    }

    if(!std::filesystem::exists(this->pkgPath)) {
        std::filesystem::create_directory(this->pkgPath);
        mustDownload = true;
    }

    return (mustDownload) ? 1 : 0;
}

int NxpmManager::checkAndDownloadNxpmDb() {
    if(!std::filesystem::exists(this->nxdbPath)) {
        std::string gitRepo = "Matographo/Nexis-NXPM-Database";
        std::string path = this->nxdbPath;
        return downloader.downloadGit(gitRepo, path);
    }
    return 0;
}

int NxpmManager::loadPackageManager() {
    std::string path = this->pmPath + "/" + this->target + ".so";
    if(!std::filesystem::exists(path)) {
        sqlite3_open(this->nxdbPath.c_str(), &this->db);
        std::string sqlSelect = "SELECT repo FROM pm WHERE name = ?";
        
        sqlite3_prepare_v2(
            this->db,
            sqlSelect.c_str(),
            -1,
            &this->stmt,
            0
        );

        sqlite3_bind_text(this->stmt, 1, this->target.c_str(), -1, SQLITE_STATIC);
        
        if(sqlite3_step(this->stmt) != SQLITE_ROW) {
            std::cerr << "No package manager found for " << this->target << std::endl;
            return 1;
        }
        
        std::string repo = (char *)sqlite3_column_text(this->stmt, 0);
        sqlite3_finalize(this->stmt);

        this->downloader.downloadGit(repo, path);  // Hier noch implementieren dass die Datei auch richtig Heruntergeladen wird und nicht nur der Pfad erstellt wird
                                                   // Ebenfalls muss der PM gebaut werden
    }
    
    this->library = loadDynamicLibrary(path);
}

int NxpmManager::runCommand() {
    if(this->command == "install") {
        if(packages.size() == 1) {
            return library->install(packages[0]);
        } else {
            return library->install(packages);
        }
    } else if(this->command == "uninstall") {
        if(packages.size() == 1) {
            return library->uninstall(packages[0]);
        } else {
            return library->uninstall(packages);
        }
    } else if(this->command == "update") {
        if(packages.size() == 0) {
            return library->update();
        } else if(packages.size() == 1) {
            return library->update(packages[0]);
        } else {
            return library->update(packages);
        }
    } else if(this->command == "search") {
        if(packages.size() == 1) {
            return library->search(packages[0]);
        } else {
            std::cerr << "Only one package can be searched for at a time" << std::endl;
            return 1;
        }
    } else if(this->command == "list") {
        return library->list();
    } 
    return 1;
}

int NxpmManager::checkAndInstallTools() {
    std::string cmakeString = "cmake --version > /dev/null 2>&1";
    std::string makeString  = "make --version > /dev/null 2>&1";
    std::string gitString   = "git --version > /dev/null 2>&1";
    std::string gppString   = "g++ --version > /dev/null 2>&1";
    
    sys sys;
    
    if(std::system(gitString.c_str()) != 0) {
        if(sys.install("git") != 0) {
            std::cerr << "Failed to install git" << std::endl;
            return 1;
        }
    }
    
    if(std::system(cmakeString.c_str()) != 0) {
        if(sys.install("cmake") != 0) {
            std::cerr << "Failed to install cmake" << std::endl;
            return 1;
        }
    }

    if(std::system(makeString.c_str()) != 0) {
        if(sys.install("make") != 0) {
            std::cerr << "Failed to install make" << std::endl;
            return 1;
        }
    }

    if(std::system(gppString.c_str()) != 0) {
        if(sys.install("g++") != 0) {
            std::cerr << "Failed to install g++" << std::endl;
            return 1;
        }
    } 
    return 0;
}