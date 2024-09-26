#include <iostream>
#include <filesystem>
#include "argparse.hpp"
#include "DynamicLibraryLoader.h"
#include "PackageManager.h"
#include "Downloader.h"

int main (int argc, char *argv[]) {
  std::string homePath = getenv("HOME");
  Downloader downloader;

  argparse::ArgumentParser program("nxpm");
  argparse::ArgumentParser installer("install");
  argparse::ArgumentParser uninstaller("uninstall");
  argparse::ArgumentParser updater("update");
  argparse::ArgumentParser searcher("search");
  argparse::ArgumentParser lister("list");
  argparse::ArgumentParser finder("find");
  
  installer.add_argument("package")
    .nargs(argparse::nargs_pattern::at_least_one)
    .help("The package to install");
  
  uninstaller.add_argument("package")
    .nargs(argparse::nargs_pattern::at_least_one)
    .help("The package to uninstall");

  updater.add_argument("package")
    .nargs(argparse::nargs_pattern::any)
    .help("The package to update");

  searcher.add_argument("package")
    .nargs(argparse::nargs_pattern::at_least_one)
    .help("The package to search for");
  
  lister.add_argument("package")
    .nargs(0)
    .help("All packages installed");
  
  finder.add_argument("package")
    .nargs(1)
    .help("The package to find");

  
  program.add_argument("target")
    .help("The target to run the command on");
  
  program.add_subparser(installer);
  program.add_subparser(uninstaller);
  program.add_subparser(updater);
  program.add_subparser(searcher);
  program.add_subparser(lister);
  program.add_subparser(finder);
  
  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    exit(0);
  }
  
  std::string target = program.get<std::string>("target");
  std::string command = "";
  std::vector<std::string> packages;
  if(program.is_subcommand_used("install")) {
    command = "install";
    packages = installer.get<std::vector<std::string>>("package");
  } else if(program.is_subcommand_used("uninstall")) {
    command = "uninstall";
    packages = uninstaller.get<std::vector<std::string>>("package");
  } else if(program.is_subcommand_used("update")) {
    command = "update";
    packages = updater.get<std::vector<std::string>>("package");
  } else if(program.is_subcommand_used("search")) {
    command = "search";
    packages = searcher.get<std::vector<std::string>>("package");
  } else if(program.is_subcommand_used("list")) {
    command = "list";
  } else if(program.is_subcommand_used("find")) {
    command = "find";
    packages = finder.get<std::vector<std::string>>("package");
  }
  
  std::string nexisPath = homePath + "/.nxpm";
  
  if(!std::filesystem::exists(nexisPath)) {
    std::filesystem::create_directories(nexisPath);
    std::filesystem::create_directories(nexisPath + "/databases");
    std::filesystem::create_directories(nexisPath + "/packages");
    std::filesystem::create_directories(nexisPath + "/package-managers");
    
    downloader.downloadGit("Matographo/Nexis-NXPM-Database", nexisPath + "/databases");
  }
  
  std::string path = homePath + "/.nxpm/package-managers";
  path += "/" + target + ".so";
  if(!std::filesystem::exists(path)) {
    std::cout << "No package manager found for " << target << std::endl;
    exit(1);
  }   

  PackageManager * library = loadDynamicLibrary(path);
  int result = 0;
  
  if(command == "install") {
    if(packages.size() == 1) {
      result = library->install(packages[0]);
    } else {
      result = library->install(packages);
    }
  } else if(command == "uninstall") {
    if(packages.size() == 1) {
      result = library->uninstall(packages[0]);
    } else {
      result = library->uninstall(packages);
    }
  } else if(command == "update") {
    if(packages.size() == 0) {
      result = library->update();
    } else if(packages.size() == 1) {
      result = library->update(packages[0]);
    } else {
      result = library->update(packages);
    }
  } else if(command == "search") {
      if(packages.size() == 1) {
        result = library->search(packages[0]);
      } else {
        std::cerr << "Only one package can be searched for at a time" << std::endl;
      }
  } else if(command == "list") {
    result = library->list();
  } 
  
  return result;
}
