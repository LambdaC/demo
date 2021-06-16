#include "VulkanUtil.h"
#include "VulkanFunctions.h"
#include "wsi.hpp"
#include <iostream>
#include <algorithm>

// PS:下面的描述中，方法和函数等同，实例和对象等同

VKAPI_ATTR VkBool32 VKAPI_CALL fnDebugCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char *pLayerPrefix, const char *pMsg, void *pUserDate)
{
    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        std::cout << "[VK_DEBUG_REPORT] ERROR: [" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg << std::endl;
    }
    else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        std::cout << "[VK_DEBUG_REPORT] WARNING: [" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg << std::endl;
    }
    else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        std::cout << "[VK_DEBUG_REPORT] INFORMATION:[" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg << std::endl;
    }
    else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        std::cout << "[VK_DEBUG_REPORT] PERFORMANCE: [" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg << std::endl;
    }
    else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        std::cout << "[VK_DEBUG_REPORT] DEBUG: [" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg << std::endl;
    }
    else
    {
        return VK_FALSE;
    }
    return VK_SUCCESS;
};

int main()
{
    // step 1
    // 使用Windows API的LoadLibrary方法加载vulkan-1.dll动态库，
    // 然后使用Windows API的GetProcAddress方法加载动态库中的PFN_vkGetInstanceProcAddr方法，
    // 接下来所有的Vulkan API方法都是通过PFN_vkGetInstanceProcAddr方法来加载，而不是通过Windows API的GetProcAddress方法，
    // 这么做为了方便跨平台，不管什么在哪个平台，Vulkan API都可以通过PFN_vkGetInstanceProcAddr方法来加载，
    // 但是PFN_vkGetInstanceProcAddr方法本身只能通过各平台特定的API进行加载。
    if (!VulkanUtil::LoadExportedFunctions())
    {
        // 加载失败，退出程序
        return 0;
    }

    // step 2
    // 在使用Vulkan API之前需要把用到的API加载进来。
    // Vulkan API分三大类方法：
    //      Global Level Functions
    //      Instance Level Functions
    //      Device Level Functions
    // 简单的区别就是，global的函数调用时不需要传Instance对象或者Device对象，其他两种就需要传对应的对象。
    // 加载Global Level方法
    // 具体加载了哪些方法，参考ListOfVulkanFunctions.inl
    // 如有新增需要加载的方法，也是在ListOfVulkanFunctuons.inl中配置
    if (!VulkanUtil::LoadGlobalLevelFunctions())
    {
        // 有些函数加载失败，推出程序
        return 0;
    }

    // 接下来，就是关键的三步创建过程
    // 1、Create Instance（VkInstance Object）（API:vkCreateInstance）
    // 2、Enumerate Physical Device（VkPhysicalDevice Object）（API:vkEnumeratePhysicalDevices）
    // 3、Create Device（VkDevice Object）（API:vkCreateDevice）

    VkResult result; // 存储创建函数返回的值

    // step 3-4 先看step 3-1
    // 看一下有哪些instance layer可以使用
    uint32_t layers_count = 0;
    result = VulkanFunctions::vkEnumerateInstanceLayerProperties(&layers_count, nullptr);
    if (result != VK_SUCCESS || layers_count == 0)
    {
        std::cout << "Enumerate Instance Layers Failed" << std::endl;
        return 0;
    }
    std::vector<VkLayerProperties> available_layers(layers_count);
    result = VulkanFunctions::vkEnumerateInstanceLayerProperties(&layers_count, available_layers.data());
    if (result != VK_SUCCESS || layers_count == 0)
    {
        return 0;
    }

#ifdef DEBUG
    // debug 模式下打印可以使用的instance layer的名字
    std::cout << "\nInstacne Layers: " << std::endl;
    for (auto &obj : available_layers)
    {
        std::cout << obj.layerName << std::endl;
    }
#endif

    // step 3-5 先看step 3-1
    // 看一下有哪些instance extentions可以使用
    uint32_t extensions_count = 0;
    result = VulkanFunctions::vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
    if (result != VK_SUCCESS || extensions_count == 0)
    {
        std::cout << "Enumerate Instance Extensions Failed" << std::endl;
        return 0;
    }

    std::vector<VkExtensionProperties> available_extensions(extensions_count);
    result = VulkanFunctions::vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data());
    if (result != VK_SUCCESS || extensions_count == 0)
    {
        return 0;
    }

    // 存储需要的layers的name
    std::vector<char const *> desired_layers = {
        // "VK_LAYER_KHRONOS_validation" //
    };

    // 存储需要的extensions的name
    std::vector<char const *> desired_extensions = WSI::getDesiredExtensions();
    desired_extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

#ifdef DEBUG
    std::cout << "\nInstance Extensions: " << std::endl;
    for (auto &extensions : available_extensions)
    {
        std::cout << extensions.extensionName << std::endl;
    }
#endif

    // step 3-3 先看step 3-1
    VkApplicationInfo application_info = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "MyApplicationName",
        VK_MAKE_VERSION(1, 0, 0),
        "MyEngineName",
        VK_MAKE_VERSION(1, 0, 0),
        VK_MAKE_VERSION(1, 0, 0) //
    };

    VkDebugReportCallbackEXT debug_report_callback = VK_NULL_HANDLE;
    VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info{};
    debug_report_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debug_report_callback_create_info.pfnCallback = (PFN_vkDebugReportCallbackEXT)(&fnDebugCallback);
    debug_report_callback_create_info.pUserData = nullptr;
    debug_report_callback_create_info.pNext = nullptr;
    debug_report_callback_create_info.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;

    // step 3-2 先看step 3-1
    // VkInstanceCreateInfo的使用参看下面的链接：
    // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkInstanceCreateInfo.html
    // 第四个参数是一个指向VkApplicationInfo对象的指针，这个创建可以看step 3-3
    // 第五个参数和第六个参数指定需要使用的Layer数和Extension数，具体使用看step 3-4和step 3-5
    VkInstanceCreateInfo instance_create_info = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        &debug_report_callback_create_info,
        0,
        &application_info,
        desired_layers.size(),
        desired_layers.data(),
        desired_extensions.size(),
        desired_extensions.data() //为了格式化时，下面的};不会自动放在这行后面而加的注释
    };

    // step 3-1 创建Vulkan Instance
    // 创建Vulkan Instance的API是vkCreateInstance
    // 可以通过下面链接查看API的使用：
    // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCreateInstance.html
    // 从文档中，可以看出该函数需要一个VkInstanceCreateInfo结构体的参数，所以需要先创建，看step 3-2
    VkInstance instance;
    result = VulkanFunctions::vkCreateInstance(&instance_create_info, nullptr, &instance);
    if (result != VK_SUCCESS || instance == VK_NULL_HANDLE)
    {
        std::cout << "Create Vulkan Instance Failed." << std::endl;
        return 0;
    }

    // step 3-6 加载instance level的API和一些extensions相关的API
    if (!VulkanUtil::LoadInstanceLevelFunctions(instance, desired_extensions))
    {
        return 0;
    }

    result = VulkanFunctions::vkCreateDebugReportCallbackEXT(instance, &debug_report_callback_create_info, nullptr, &debug_report_callback);
    if (result != VK_SUCCESS || debug_report_callback == VK_NULL_HANDLE)
    {
        std::cout << "Create Debug Report Callback Failed!" << std::endl;
        return 0;
    }

    // step 4 找physical device
    // 当vulkan instance创建完毕之后，就要看一下有哪些physical device（显卡）可以使用
    // 使用vkEnumeratePhysicalDevices API
    uint32_t devices_count;
    result = VulkanFunctions::vkEnumeratePhysicalDevices(instance, &devices_count, nullptr);
    if (result != VK_SUCCESS || devices_count == 0)
    {
        std::cout << "Could not find a devices that Support Vulkan";
        return 0;
    }

#ifdef DEBUG
    std::cout << "\nDevices Count: " << devices_count << std::endl;
#endif

    std::vector<VkPhysicalDevice> available_devices(devices_count);
    result = VulkanFunctions::vkEnumeratePhysicalDevices(instance, &devices_count, available_devices.data());

    // 如果机子只有一个显卡，那就使用这个显卡就行了。
    VkPhysicalDevice physical_device = available_devices[0];

    // step 5-4 先看step 5-1
    // 创建step 5-3中需要的数据
    uint32_t queue_family_property_count;
    VulkanFunctions::vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, nullptr);
    if (queue_family_property_count <= 0)
    {
        std::cout << "No queue family properties!" << std::endl;
        return 0;
    }

#ifdef DEBUG
    std::cout << "Queue family property count: " << queue_family_property_count << std::endl;
#endif

    std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_property_count);
    VulkanFunctions::vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_property_count, queue_family_properties.data());
    // 找出我们想要的queue family，即支持graphic operation的queue family，一个就够了
    uint32_t queue_family_index = queue_family_property_count;
    for (uint32_t i = 0; i < queue_family_properties.size(); i++)
    {
        auto &family = queue_family_properties[i];
        if (family.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT == VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
        {
            queue_family_index = i;
            break;
        }
    }

    if (queue_family_index == queue_family_property_count)
    {
        std::cout << "No queue family that support Graphic Operation" << std::endl;
        return 0;
    }

#ifdef DEBUG
    std::cout << "queue_family_index: " << queue_family_index << std::endl;
#endif

    // step 5-3 先看step 5-1
    // 创建logical device时会创建与之关联的queue
    VkDeviceQueueCreateInfo device_queue_create_info{};
    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_info.pNext = NULL;
    device_queue_create_info.flags = 0;
    //queueFamilyIndex is an unsigned integer indicating the index of the
    // queue family in which to create the queue on this device.
    //This index corresponds to the index of an element of the pQueueFamilyProperties array
    // that was returned by vkGetPhysicalDeviceQueueFamilyProperties.
    // 看step 5-4
    device_queue_create_info.queueFamilyIndex = 0;
    device_queue_create_info.queueCount = 1;
    device_queue_create_info.pQueuePriorities = std::vector<float>{1.0f}.data();

    // VkDeviceCreateInfo.queupQueueCreateInfos需要的是指向VkDeviceQueueCreateInfo的数组
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    queue_create_infos.push_back(device_queue_create_info);

    // step 5-2 先看step 5-1
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = NULL;
    device_create_info.flags = 0;
    device_create_info.pEnabledFeatures = nullptr;
    device_create_info.queueCreateInfoCount = queue_create_infos.size();
    device_create_info.pQueueCreateInfos = queue_create_infos.data(); // 创建logical device时会创建与之关联的queue，因为queue是操作logical device的关键，所以不要为空吧，创建看step 5-3
    device_create_info.enabledExtensionCount = 1;
    device_create_info.ppEnabledExtensionNames = WSI::getDesiredDeviceExtensions().data();

    // step 5-1 创建logical device
    // 在vulkan中，对显卡的操作基本上都是通过对logical device进行操作来进行的
    // logical device就是对显卡的抽象
    // API:vkCreateDevice
    // 从API需要的参数，我们就可以知道还需要什么结构体参数，看step 5-2
    VkDevice device;
    result = VulkanFunctions::vkCreateDevice(physical_device, &device_create_info, nullptr, &device);
    if (result != VK_SUCCESS)
    {
        std::cout << "\nCreate Logical Device Failed." << std::endl;
        return 0;
    }

    if (!VulkanUtil::LoadDeviceLevelFunctions(device, WSI::getDesiredDeviceExtensions()))
    {
        return 0;
    }

    // logcial device已经创建完了，相当于现在拥有了一个GPU，
    // 要显示渲染的内容，除了GPU之外，还需要一个显示渲染内容的窗口（或者说展示层/表示层---presentation？）
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    result = WSI::vkCreateWindowSurfaceKHR(instance, &surface);
    if (result != VK_SUCCESS || surface == VK_NULL_HANDLE)
    {
        std::cout << "Create Win32 Surface Failed!" << std::endl;
    }

    // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#vkGetPhysicalDeviceSurfaceSupportKHR
    // 打开上面的链接看这个API的作用
    // 验证选择的queue family是否支持Surface
    VkBool32 isPresentationSupported;
    VulkanFunctions::vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &isPresentationSupported);
    if (!isPresentationSupported)
    {
        std::cout << "Selected Queue Family doesn't support presentation." << std::endl;
        return 0;
    }
    // 来到这里，说明queue_family_index关联的queue支持presentation。

    // 看一下surface支持什么颜色格式
    uint32_t surface_format_count;
    result = VulkanFunctions::vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, nullptr);
    if (result != VK_SUCCESS || surface_format_count == 0)
    {
        std::cout << "No Support Format!" << std::endl;
        return 0;
    }

    std::vector<VkSurfaceFormatKHR> supported_surface_formats(surface_format_count);
    result = VulkanFunctions::vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, supported_surface_formats.data());

    // 验证一下支不支持想要的fomat，因为下面创建swapchain时需要指定format
    // VkFormat desired_format = VK_FORMAT_R8G8B8A8_UNORM;
    // VkColorSpaceKHR desired_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkSurfaceFormatKHR desired_surface_format = {
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR //
    };

    bool isSupport{false};
    for (auto &surface_format : supported_surface_formats)
    {
        if (surface_format.format == desired_surface_format.format && surface_format.colorSpace == desired_surface_format.colorSpace)
        {
            isSupport = true;
            break;
        }
    }

    if (!isSupport)
    {
        std::cout << "No desired surface format supported!" << std::endl;
        return 0;
    }

    // 操作显卡的命令，需要用到command、command buffer、command pool等相关的东西
    // 创建command pool，使用vkCreateCommandPool API
    VkCommandPool command_pool = VK_NULL_HANDLE;
    VkCommandPoolCreateInfo command_pool_create_info{};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = NULL;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_create_info.queueFamilyIndex = queue_family_index; // 创建command pool时需要指定与哪个queue关联的。
    result = VulkanFunctions::vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool);
    if (result != VK_SUCCESS || command_pool == VK_NULL_HANDLE)
    {
        std::cout << "Create Command Pool Failed!" << std::endl;
        return 0;
    }

    // 创建一个command buffer，使用vkAllocateCommandBuffers
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    VkCommandBufferAllocateInfo command_buffer_allocate_info{};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.pNext = NULL;
    command_buffer_allocate_info.commandPool = command_pool;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = 1;
    result = VulkanFunctions::vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer);
    if (result != VK_SUCCESS || command_buffer == VK_NULL_HANDLE)
    {
        std::cout << "Create Command Buffer Failed!" << std::endl;
        return 0;
    }

    // 创建SwapChain

    // 先Query一下surface支持的功能/特性
    VkSurfaceCapabilitiesKHR surface_capabilities;
    result = VulkanFunctions::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);
    if (result != VK_SUCCESS)
    {
        std::cout << "Get Surface Capabilities Failed!" << std::endl;
        return 0;
    }
    /**
     * Choosing a size of swapchain images
     */
    VkExtent2D size_of_images = {1920, 1080};
    // 是否支持窗口的大小由surface的属性来定
    if (surface_capabilities.currentExtent.width == 0xFFFFFFFF)
    {
        size_of_images = {1920, 1080};
    }
    size_of_images.width = max(size_of_images.width, surface_capabilities.minImageExtent.width);
    size_of_images.width = min(size_of_images.width, surface_capabilities.maxImageExtent.width);
    size_of_images.height = max(size_of_images.height, surface_capabilities.minImageExtent.width);
    size_of_images.height = min(size_of_images.height, surface_capabilities.maxImageExtent.height);

    /**
     * Selecting desired usage scenarios of swapchain images
     */
    VkImageUsageFlags desired_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (!(desired_usage & surface_capabilities.supportedUsageFlags))
    {
        std::cout << "Surface Can not support some Image Usage!" << std::endl;
        return 0;
    }

    /**
     * Selecting a transformation of swapchain images
     */
    VkSurfaceTransformFlagBitsKHR surface_transform;
    VkSurfaceTransformFlagBitsKHR desired_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    if (desired_transform & surface_capabilities.supportedTransforms)
    {
        surface_transform = desired_transform;
    }
    else
    {
        surface_transform = surface_capabilities.currentTransform;
    }

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkSwapchainKHR old_swapchain = VK_NULL_HANDLE;

    VkSwapchainCreateInfoKHR swapchain_create_info{};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = 2u;
    swapchain_create_info.imageFormat = desired_surface_format.format;
    swapchain_create_info.imageColorSpace = desired_surface_format.colorSpace;
    swapchain_create_info.imageExtent = size_of_images;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = desired_usage;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.preTransform = desired_transform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR; // 其实还是需要使用vkGetPhysicalDeviceSurfacePresentModesKHR API去获取一下支持哪些mode的
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = old_swapchain;
    result = VulkanFunctions::vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain);
    if (result != VK_SUCCESS || swapchain == VK_NULL_HANDLE)
    {
        std::cout << "Create Swapchain Failed!" << std::endl;
        return 0;
    }

    uint32_t swapchain_image_count{0};
    result = VulkanFunctions::vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr);
    std::vector<VkImage> swapchain_images(swapchain_image_count);
    result = VulkanFunctions::vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images.data());

    return 0;
}