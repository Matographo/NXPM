#include "DynamicLibraryLoader.h"

PackageManager * loadDynamicLibrary(std::string path) {
    void * handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load dynamic library: " << std::string(dlerror()) << std::endl;
        return nullptr;
    }

    PackageManager * (*create)();
    create = (PackageManager * (*)())dlsym(handle, "create");
    if (!create) {
        std::cerr << "Failed to load symbol create: " << std::string(dlerror());
        dlclose(handle);
        return nullptr;
    }

    PackageManager * plugin = create();
    if (!plugin) {
        std::cerr << "Failed to create instance" << std::endl;
        return nullptr;
    }

    return plugin;
}