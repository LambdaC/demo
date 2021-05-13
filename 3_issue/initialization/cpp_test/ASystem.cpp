#include "ASystem.h"
#include <iostream>
#include "LogSystem.h"

extern LogSystem logSystem;

ASystem::ASystem(){
    std::cout << "Create ASystem" << std::endl;
    logSystem.log();
};

ASystem aSysetm;

