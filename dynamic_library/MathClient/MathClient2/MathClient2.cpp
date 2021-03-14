// MathClient2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h> 
#include <iostream>

typedef void(__cdecl* fibonacci_init)(int, int);
typedef bool(_cdecl* fibonacci_next)();
typedef unsigned long long(_cdecl* fibonacci_current)();
typedef unsigned(_cdecl* fibonacci_index)();

int main(void)
{
    HINSTANCE hinstLib;
    fibonacci_init fib_init;
    fibonacci_next fib_next;
    fibonacci_current fib_current;
    fibonacci_index fib_index;

    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

    std::cout << "Using DLL by using Run-Time Dynamic Linking" << std::endl;

    // Get a handle to the DLL module.

    hinstLib = LoadLibrary(TEXT("MathLibrary.dll"));

    // If the handle is valid, try to get the function address.

    if (hinstLib != NULL)
    {
        fib_init = (fibonacci_init)GetProcAddress(hinstLib, "fibonacci_init");
        fib_next = (fibonacci_next)GetProcAddress(hinstLib, "fibonacci_next");
        fib_current = (fibonacci_current)GetProcAddress(hinstLib, "fibonacci_current");
        fib_index = (fibonacci_index)GetProcAddress(hinstLib, "fibonacci_index");

        // If the function address is valid, call the function.

        if (NULL != fib_init && NULL != fib_next && NULL != fib_current && NULL != fib_index)
        {
            fRunTimeLinkSuccess = TRUE;
            (fib_init)(1, 1);
            do {
                std::cout << (fib_index)() << ": "
                    << (fib_current)() << std::endl;
            } while ((fib_next)());
            std::cout << (fib_index)() + 1 <<
                " Fibonacci sequence values fit in an " <<
                "unsigned 64-bit integer." << std::endl;
        }
        // Free the DLL module.

        fFreeResult = FreeLibrary(hinstLib);
    }

    // If unable to call the DLL function, use an alternative.
    if (!fRunTimeLinkSuccess)
        std::cout << "Message printed from executable\n";

    return 0;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
