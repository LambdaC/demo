#ifndef VULKAN_UTIL_HEADER
#define VULKAN_UTIL_HEADER
#include <vector>
#include <Windows.h>
#include "vulkan/vulkan.h"
namespace VulkanUtil
{
    extern HMODULE vulkan_library;
    bool LoadExportedFunctions();
    bool LoadGlobalLevelFunctions();
    bool LoadInstanceLevelFunctions(VkInstance &instance,
                                    std::vector<const char *> const &enabled_extensions);
    bool LoadDeviceLevelFunctions(VkDevice logical_device,
                                  std::vector<char const *> const &enabled_extensions);
}

#endif