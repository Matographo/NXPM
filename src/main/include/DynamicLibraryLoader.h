#ifndef DYANMIC_LIBRARY_LOADER_H
#define DYANMIC_LIBRARY_LOADER_H

#include <string>
#include <iostream>
#include <dlfcn.h>
#include "PackageManager.h"

PackageManager * loadDynamicLibrary(std::string path);

#endif