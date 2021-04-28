#include "LogSystem.h"
#include <iostream>

LogSystem::LogSystem() :i{100} {
    std::cout << "Create LogSystem" << std::endl;
};

void LogSystem::log() {
    std::cout << "log:" << i << std::endl;
};

LogSystem logSystem;