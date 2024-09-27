#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <string>
#include <vector>

/**
 * Interface for package managers
*/
class PackageManager {
private:

public:
    virtual ~PackageManager() = default;
  
    /**
     * Install a given package
     * @param package The package to install or the package in addition with the version
     * @return 0 if successful, 1 if not
     */
    virtual int install(std::string package) = 0;
    
    /**
     * Install a list of packages or the packages in addition with the version
     * @param packages The list of packages to install
     * @return 0 if successful, 1 if not
     */
    virtual int install(std::vector<std::string> packages) = 0;
    
    /**
     * Uninstall a given package or the package in addition with the version
     * @param package The package to uninstall
     * @return 0 if successful, 1 if not
     */
    virtual int uninstall(std::string package) = 0;
    
    /**
     * Uninstall a list of packages or the packages in addition with the version
     * @param packages The list of packages to uninstall
     * @return 0 if successful, 1 if not
     */
    virtual int uninstall(std::vector<std::string> packages) = 0;
    
    /**
     * Update a given package or the package in addition with the version
     * @param package The package to update
     * @return 0 if successful, 1 if not
     */
    virtual int update(std::string package) = 0;
    
    /**
     * Update a list of packages or the packages in addition with the version
     * @param packages The list of packages to update
     * @return 0 if successful, 1 if not
     */
    virtual int update(std::vector<std::string> packages) = 0;
    
    /**
     * Update all packages
     * @return 0 if successful, 1 if not
     */
    virtual int update() = 0;
    
    /**
     * Search for a given package
     * @param package The package to search for
     * @return 0 if successful, 1 if not
     */
    virtual int search(std::string package) = 0;
    
    /**
     * List all packages installed on the system
     * @return 0 if successful, 1 if not
     */
    virtual int list() = 0;
    
    /**
     * Get information about a given package
     * @param package The package to get information about
     * @return 0 if successful, 1 if not
     */
    virtual int info(std::string package) = 0;
};

#endif