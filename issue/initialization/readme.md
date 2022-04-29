ASystem.cpp
```c++
extern LogSystem logSystem;

ASystem::ASystem(){
    std::cout << "Create ASystem" << std::endl;
    logSystem.log();
};

ASystem aSysetm;
```

LogSystem.cpp

```c++
LogSystem::LogSystem() :i{100} {
    std::cout << "Create LogSystem" << std::endl;
};

void LogSystem::log() {
    std::cout << "log:" << i << std::endl;
};

LogSystem logSystem;
```

如果aSystem比logSystem先初始化，那么为什么aSystem的构造函数还能调用log函数，能调用其实我也能理解，但是log里面会引用到i变量，如果logSystem没有初始化（是不是相当于没有创建），那么怎么可以访问到i变量呢？能访问到i变量，说明logSystem对象已经存在了。这是否说明logSystem的创建（分配内存空间）和初始化是分开的？