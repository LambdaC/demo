#include <iostream>
#include <Windows.h>
#include "VulkanUtil.h"
#include "VulkanFunctions.h"

namespace VulkanUtil
{
    HMODULE vulkan_library;
    bool LoadExportedFunctions()
    {
#define EXPORTED_VULKAN_FUNCTION(name)                                                \
    vulkan_library = LoadLibrary(TEXT("vulkan-1.dll"));                               \
    if (vulkan_library == nullptr)                                                    \
    {                                                                                 \
        std::cout << "Could not connect with a Vulkan Runtime library." << std::endl; \
        return false;                                                                 \
    }                                                                                 \
    VulkanFunctions::name = (PFN_##name)GetProcAddress(vulkan_library, #name);        \
    if (VulkanFunctions::name == nullptr)                                             \
    {                                                                                 \
        std::cout << "Load name function failed!" << std::endl;                       \
        return false;                                                                 \
    }
#include "ListOfVulkanFunctions.inl"
        return true;
    }

    bool LoadGlobalLevelFunctions()
    {
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                                      \
    VulkanFunctions::name = (PFN_##name)VulkanFunctions::vkGetInstanceProcAddr(nullptr, #name); \
    if (VulkanFunctions::name == nullptr)                                                       \
    {                                                                                           \
        std::cout << "Could not load global-level function named: " << #name << std::endl;      \
        return false;                                                                           \
    }

#include "ListOfVulkanFunctions.inl"
        return true;
    }

    bool LoadInstanceLevelFunctions(VkInstance &instance, std::vector<const char *> const &enabled_extensions)
    {
        // Load core Vulkan API instance-level functions
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                                     \
    VulkanFunctions::name = (PFN_##name)VulkanFunctions::vkGetInstanceProcAddr(instance, #name); \
    if (VulkanFunctions::name == nullptr)                                                        \
    {                                                                                            \
        std::cout << "Could not load instance-level Vulkan function named: " #name << std::endl; \
        return false;                                                                            \
    }

        // Load instance-level functions from enabled extensions
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                                   \
    for (auto &enabled_extension : enabled_extensions)                                                   \
    {                                                                                                    \
        if (std::string(enabled_extension) == std::string(extension))                                    \
        {                                                                                                \
            VulkanFunctions::name = (PFN_##name)VulkanFunctions::vkGetInstanceProcAddr(instance, #name); \
            if (VulkanFunctions::name == nullptr)                                                        \
            {                                                                                            \
                std::cout << "Could not load instance-level Vulkan function named: " #name << std::endl; \
                return false;                                                                            \
            }                                                                                            \
        }                                                                                                \
    }

#include "ListOfVulkanFunctions.inl"

        return true;
    }

    bool LoadDeviceLevelFunctions(VkDevice logical_device,
                                  std::vector<char const *> const &enabled_extensions)
    {
        // Load core Vulkan API device-level functions
#define DEVICE_LEVEL_VULKAN_FUNCTION(name)                                                           \
    VulkanFunctions::name = (PFN_##name)VulkanFunctions::vkGetDeviceProcAddr(logical_device, #name); \
    if (VulkanFunctions::name == nullptr)                                                            \
    {                                                                                                \
        std::cout << "Could not load device-level Vulkan function named: " #name << std::endl;       \
        return false;                                                                                \
    }

        // Load device-level functions from enabled extensions
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                                         \
    for (auto &enabled_extension : enabled_extensions)                                                       \
    {                                                                                                        \
        if (std::string(enabled_extension) == std::string(extension))                                        \
        {                                                                                                    \
            std::cout << "Loading: " << enabled_extension << std::endl;                                      \
            VulkanFunctions::name = (PFN_##name)VulkanFunctions::vkGetDeviceProcAddr(logical_device, #name); \
            if (VulkanFunctions::name == nullptr)                                                            \
            {                                                                                                \
                std::cout << "Could not load device-level Vulkan function named: " #name << std::endl;       \
                return false;                                                                                \
            }                                                                                                \
        }                                                                                                    \
    }

#include "ListOfVulkanFunctions.inl"

        return true;
    }
}