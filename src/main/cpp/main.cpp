#include "NxpmManager.h"

/**
 * Main function to start and control the program
 */
int main (int argc, char **argv) {
  NxpmManager * nxpmManager = new NxpmManager(argc, argv);
  
  int result = 0;
  if(result == 0) result = nxpmManager->parseArguments();
  if(result == 0) result = nxpmManager->checkAndInstallTools();
  if(result == 0) result = nxpmManager->checkAndCreatePaths();
  if(result == 0) result = nxpmManager->checkAndDownloadNxpmDb();
  if(result == 0) result = nxpmManager->loadPackageManager();
  if(result == 0) result = nxpmManager->findMissingPackages();
  if(result == 0) result = nxpmManager->runCommand();
  
  return result;
}
