#include "wsi.hpp"
#include <windows.h>
#include "vulkan/vulkan_win32.h"
#include "VulkanFunctions.h"
#include <iostream>

HINSTANCE hInstance;
HWND hWnd;

VkResult WSI::vkCreateWindowSurfaceKHR(VkInstance instance, VkSurfaceKHR *pSurface)
{

    hInstance = GetModuleHandle(nullptr); // 参数为nullptr则返回该程序实例的handle

    const TCHAR CLASS_NAME[] = TEXT("Sample Window Class");

    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);
    // Create the window.
    hWnd = CreateWindowEx(
        0,                                // Optional window styles.
        CLASS_NAME,                       // Window class
        TEXT("Learn to Program Windows"), // Window text
        WS_OVERLAPPEDWINDOW,              // Window style
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        NULL       // Additional application data
    );

    VkWin32SurfaceCreateInfoKHR surface_create_info = {
        VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        nullptr,
        0,
        hInstance,
        hWnd //
    };

    // PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)VulkanFunctions::vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
    VkResult result = VulkanFunctions::vkCreateWin32SurfaceKHR(instance, &surface_create_info, nullptr, pSurface);
    return result;
}

std::vector<const char *> WSI::getDesiredExtensions()
{
    std::vector<const char *> reslt;
    reslt.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
    reslt.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    return reslt;
}

std::vector<const char *> WSI::getDesiredDeviceExtensions()
{
    std::vector<const char *> reslt;
    reslt.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    return reslt;
}
