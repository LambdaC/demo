### 动态链接库的创建和使用例子

参考链接：

https://docs.microsoft.com/en-us/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp?view=msvc-160

MathClient项目中使用DLL的方法叫做[Load-Time Dynamic Linking](https://docs.microsoft.com/en-us/windows/win32/dlls/using-load-time-dynamic-linking)。就是需要使用DLL提供的配套lib库和头文件。lib库实现的应该就是load dll的过程吧。

MathClient2项目中使用DLL的方法叫做[Using Run-Time Dynamic Linking](https://docs.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking)。做法就是使用Windows API提供的LoadLibrary 方法把DLL在运行时加载进来，然后再使用Windows API提供的GetProcAddress方法把需要用到的函数读取出来使用。

方法2会用到函数指针，函数指针的用法可以参考下面这个链接。

https://stackoverflow.com/questions/840501/how-do-function-pointers-in-c-work