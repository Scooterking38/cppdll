#include <iostream>
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern "C" EXPORT void exampleFunction() {
    std::cout << "Hello from DLL!" << std::endl;
}
