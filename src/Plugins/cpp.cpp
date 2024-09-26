#include "cpp.h"

Cpp::Cpp() {
    sqlite3 *db;
    
    std::string path = "/home/leodora/.nxpm/databases";
    if(!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
    path += "/cpp.db";
    if(!std::filesystem::exists(path)) {
        sqlite3_open(path.c_str(), &db);
        sqlite3_exec(db, "CREATE TABLE packages (name TEXT, repo TEXT", NULL, NULL, NULL);
        sqlite3_close(db);
    }
    
    std::string pathToPackage = "/home/leodora/.nxpm/packages/cpp";
    if(!std::filesystem::exists(pathToPackage)) {
        std::filesystem::create_directories(pathToPackage);
    }
    
    this->db = db;
    sqlite3_open(path.c_str(), &this->db);
}

Cpp::~Cpp() {
    sqlite3_close(this->db);
}

int Cpp::install(std::string package) {
    std::string packageName;
    std::string version;
    bool isHash = false;

    if(package.find("@") != std::string::npos) {
        packageName = package.substr(0, package.find("@"));
        version = package.substr(package.find("@") + 1);
    } else if(package.find("#") != std::string::npos) {
        packageName = package.substr(0, package.find("#"));
        version = package.substr(package.find("#") + 1);
        isHash = true;
    } else {
        std::cerr << "No version specified for package " << package << std::endl;
        return 1;
    }
    std::string pathToPackage = "/home/leodora/.nxpm/packages/cpp/" + packageName;
    std::string pathToPackageVersion = pathToPackage + "/" + version;

    if(std::filesystem::exists(pathToPackageVersion + "/include") && std::filesystem::exists(pathToPackageVersion + "/cpp")
        && std::filesystem::is_directory(pathToPackageVersion + "/include") && std::filesystem::is_directory(pathToPackageVersion + "/cpp")) {
        return 0;
    }
    
    if(!std::filesystem::exists(pathToPackage)) {
        std::filesystem::create_directories(pathToPackage);
    }
    
    if(std::filesystem::exists(pathToPackage + "/.raw")) {
        return this->createNewVersion(version, isHash, packageName, pathToPackage, pathToPackageVersion);
    }
    
    std::string sqlSelect = "SELECT * FROM packages WHERE name = ?";
    
    int sqlResult = sqlite3_prepare_v2(
        this->db,
        sqlSelect.c_str(),
        -1,
        &this->stmt,
        0
    );
    
    if(sqlResult != SQLITE_OK) {
        std::cerr << "Failed to prepare SQL statement" << std::endl;
        return 1;
    }
    
    sqlite3_bind_text(this->stmt, 1, packageName.c_str(), -1, SQLITE_STATIC);
    
    if(sqlite3_step(this->stmt) != SQLITE_ROW) {
        std::cerr << "No package found with name " << packageName << std::endl;
        return 1;
    }

    std::string repo = (char *)sqlite3_column_text(this->stmt, 1);
    
    sqlite3_finalize(this->stmt);

    if(repo == "") {
        std::cerr << "No package found with name " << packageName << std::endl;
        return 1;
    }
    Downloader downloader;
    if(downloader.downloadGit(repo, pathToPackage + "/.raw") != 0) {
        std::cerr << "Failed to download package " << packageName << std::endl;
        return 1;
    }
    
    return this->createNewVersion(version, isHash, packageName, pathToPackage, pathToPackageVersion);
}

int Cpp::install(std::vector<std::string> packages) {
    int result = 0;
    for(std::string package : packages) {
        result += this->install(package);
    }
    return result;
}

int Cpp::uninstall(std::string package) {
    std::string packageName;
    std::string version;
    bool isHash = false;

    std::string pathToPackage = "/home/leodora/.nxpm/packages/cpp/" + packageName;

    if(package.find("@") != std::string::npos) {
        packageName = package.substr(0, package.find("@"));
        version = package.substr(package.find("@") + 1);
    } else if(package.find("#") != std::string::npos) {
        packageName = package.substr(0, package.find("#"));
        version = package.substr(package.find("#") + 1);
        isHash = true;
    } else {
        std::filesystem::remove_all(pathToPackage);
        std::cout << "Package " << packageName << " uninstalled" << std::endl;
        return 0;
    }
    std::string pathToPackageVersion = pathToPackage + "/" + version;

    std::filesystem::remove_all(pathToPackageVersion);
    
    bool found = false;
    for(const auto & entry : std::filesystem::directory_iterator(pathToPackage)) {
        if(entry.path().filename() != ".raw") {
            found = true;
            break;
        }
    }
    
    if(!found) {
        std::filesystem::remove_all(pathToPackage);
    }
    
    return 0;
}

int Cpp::uninstall(std::vector<std::string> packages) {
    int result = 0;
    for(std::string package : packages) {
        result += this->uninstall(package);
    }
    return result;
}

int Cpp::update(std::string package) {
    std::string pathToPackage = "/home/leodora/.nxpm/packages/cpp/" + package;
    if(!std::filesystem::exists(pathToPackage + "/.raw")) {
        std::cerr << "No package found with name " << package << std::endl;
        return 1;
    } else {
        std::string gitCommand = "git -C " + pathToPackage + "/.raw pull";
        int result = system(gitCommand.c_str());
        if(result != 0) {
            std::cerr << "Failed to update package " << package << std::endl;
            return 1;
        }
    }
    return 0;
}

int Cpp::update(std::vector<std::string> packages) {
    int result = 0;
    for(std::string package : packages) {
        result += this->update(package);
    }
    return result;
}

int Cpp::update() {
    std::string pathToPackage = "/home/leodora/.nxpm/packages/cpp";
    std::vector<std::string> packages;
    for(const auto & entry : std::filesystem::directory_iterator(pathToPackage)) {
        packages.push_back(entry.path().filename());
    }
    return this->update(packages);
}

int Cpp::search(std::string package) {
    std::string sqlSelect = "SELECT * FROM packages WHERE name = ?";
    
    sqlite3_prepare_v2(
        this->db,
        sqlSelect.c_str(),
        -1,
        &this->stmt,
        0
    );
    
    sqlite3_bind_text(this->stmt, 1, package.c_str(), -1, SQLITE_STATIC);
    
    bool found = false;
    
    while(sqlite3_step(this->stmt) == SQLITE_ROW) {
        std::cout << "Package " << (char *)sqlite3_column_text(this->stmt, 0) << " found at " << (char *)sqlite3_column_text(this->stmt, 1) << std::endl;
        found = true;
    }

    sqlite3_finalize(this->stmt);

    if(!found) {
        std::cerr << "No package found with name " << package << std::endl;
        return 1;
    }
    
    return 0;
}

int Cpp::list() {
    std::string pathToPackages = "/home/leodora/.nxpm/packages/cpp";
    
    for(const auto & entry : std::filesystem::directory_iterator(pathToPackages)) {
        std::cout << entry.path().filename() << std::endl;
    }
    
    return 0;
}

int Cpp::info(std::string package) {
    std::string sqlSelect = "SELECT * FROM packages WHERE name = ?";
    
    sqlite3_prepare_v2(
        this->db,
        sqlSelect.c_str(),
        -1,
        &this->stmt,
        0
    );
    
    sqlite3_bind_text(this->stmt, 1, package.c_str(), -1, SQLITE_STATIC);
    
    sqlite3_step(this->stmt);

    std::string repo = (char *)sqlite3_column_text(this->stmt, 1);
    
    sqlite3_finalize(this->stmt);

    if(repo == "") {
        std::cerr << "No package found with name " << package << std::endl;
        return 1;
    }
    
    std::string pathToPackage = "/home/leodora/.nxpm/packages/cpp/" + package;
    
    if(!std::filesystem::exists(pathToPackage)) {
        std::cerr << "No package " << package << " is installed" << std::endl;
    }
    
    if(!std::filesystem::exists(pathToPackage + "/.raw")) {
        std::cerr << "No package found with name " << package << std::endl;
    }     
    
    std::string gitCommand = "git -C " + pathToPackage + "/.raw checkout HEAD";
    
    if(system(gitCommand.c_str()) != 0) {
        std::cerr << "Failed to checkout HEAD for package " << package << std::endl;
        return 1;
    }
    
    std::stringstream output;
    
    output << "Package: " << package << std::endl;
    output << "Repository: " << repo << std::endl;

    if(std::filesystem::exists(pathToPackage + "/README.md")) {
        output << "README:" << std::endl;
        std::ifstream readme(pathToPackage + "/README.md");
        output << readme.rdbuf();
        readme.close();
    }
    
    return 0;
}


int Cpp::createNewVersion(std::string version, bool isHash, std::string packageName, std::string pathToPackage, std::string pathToPackageVersion) {
    std::filesystem::create_directories(pathToPackageVersion);
    
    std::string gitCommand = "git -C " + pathToPackage + "/.raw ";

    if(isHash) {
        gitCommand += "checkout " + version;
    } else {
        gitCommand += "checkout tags/" + version;
    }
    
    if(system(gitCommand.c_str()) != 0) {
        std::cerr << "Failed to checkout version " << version << " for package " << packageName << std::endl;
        std::filesystem::remove_all(pathToPackageVersion);
        return 1;
    }

    std::string pathToRepo = pathToPackage + "/.raw";
    std::string pathToBuild = pathToPackage + "/build";
    
    std::filesystem::create_directories(pathToPackageVersion + "/include");
    std::filesystem::create_directories(pathToPackageVersion + "/cpp");
    std::filesystem::create_directories(pathToBuild);

    if(std::filesystem::exists(pathToRepo + "/include")) {
        std::filesystem::copy(pathToRepo + "/include", pathToPackageVersion + "/include", std::filesystem::copy_options::recursive);
    } else {
        for(const auto & entry : std::filesystem::directory_iterator(pathToRepo)) {
            if(entry.path().extension() == ".h" || entry.path().extension() == ".hpp") {
                std::filesystem::copy(entry.path(), pathToPackageVersion + "/include");
            }
        }
    }
    
    std::string cmakeBuild = "cmake -S " + pathToRepo + " -B " + pathToBuild;
    
    if(system(cmakeBuild.c_str()) != 0) {
        std::cerr << "Failed to generate build files for package " << packageName << std::endl;
        std::filesystem::remove_all(pathToPackageVersion);
        std::filesystem::remove_all(pathToBuild);
        return 1;
    }
    
    std::string makeBuild = "make -C " + pathToBuild;
    
    if(system(makeBuild.c_str()) != 0) {
        std::cerr << "Failed to build package " << packageName << std::endl;
        std::filesystem::remove_all(pathToPackageVersion);
        std::filesystem::remove_all(pathToBuild);
        return 1;
    }
    
    std::string libraryFile;
    std::string libraryExtention;

    for(const auto & entry : std::filesystem::directory_iterator(pathToBuild)) {
        libraryExtention = entry.path().extension();
        if(libraryExtention == ".so" || libraryExtention == ".a" || libraryExtention == ".dll" || libraryExtention == ".lib") {
            libraryFile = entry.path();
            break;
        }
    }
    if(libraryFile == "") {
        std::cerr << "No library file found for package " << packageName << std::endl;
        std::filesystem::remove_all(pathToPackageVersion);
        std::filesystem::remove_all(pathToBuild);
        return 1;
    }
    
    std::filesystem::copy(libraryFile, pathToPackageVersion + "/cpp");
    
    std::filesystem::remove_all(pathToBuild);
    return 0;
}

extern "C" PackageManager * create() {
    return new Cpp();
}

extern "C" void destroy(PackageManager * manager) {
    delete manager;
}