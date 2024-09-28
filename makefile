start:
	g++ -g -Isrc/main/include -Isrc/libs/ArgParse/include -o build/result/Nexp src/main/cpp/main.cpp src/main/cpp/DynamicLibraryLoader.cpp src/main/cpp/NxpmManager.cpp src/libs/sqlite.o src/main/cpp/Downloader.cpp src/main/cpp/sys.cpp
	
plugin:
	g++ -g -Isrc/main/include -Isrc/Plugins -fPIC -shared -o build/result/sys.so src/Plugins/sys.cpp
	
plugin2:
	g++ -g -Isrc/main/include -Isrc/Plugins -fPIC -shared -o build/result/cpp.so src/Plugins/cpp.cpp src/libs/sqlite.o src/main/cpp/Downloader.cpp