#include "vulkan/vulkan.h"
#include <vector>

namespace WSI
{
    VkResult vkCreateWindowSurfaceKHR(VkInstance instance, VkSurfaceKHR *pSurface);

    std::vector<const char *> getDesiredExtensions();

    std::vector<const char *> getDesiredDeviceExtensions();
}