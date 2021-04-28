#include <iostream>
#include "LogSystem.h"

extern LogSystem logSystem;

int main() {
    logSystem.log();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}