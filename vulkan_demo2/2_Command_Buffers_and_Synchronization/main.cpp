#include <iostream>
#include <vector>
#include <bitset>
#include <Windows.h>
#include <WinUser.h>
#include <algorithm>
#include "vulkan\vulkan_core.h"
#include "vulkan\vulkan_win32.h"
#include "VulkanUtil.h"
#include "VulkanFunctions.h"

int main()
{

    if (!VulkanUtil::LoadExportedFunctions())
    {
        return 0;
    }
    std::cout << "Load ExportedFunctions Succ" << std::endl;

    if (!VulkanUtil::LoadGlobalLevelFunctions())
    {
        return 0;
    }
    std::cout << "Load GlobalLevelFunctions Succ" << std::endl;

    /**
     * check available extensions
     */
    uint32_t extensions_count = 0;
    VkResult result = VK_SUCCESS;
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
    std::cout << "\nInstance Extensions: " << std::endl;
    for (auto &extensions : available_extensions)
    {
        std::cout << extensions.extensionName << std::endl;
    }

    /**
     * check available layers
     */
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
    std::cout << "\nInstacne Layers: " << std::endl;
    for (auto &obj : available_layers)
    {
        std::cout << obj.layerName << std::endl;
    }

    /**
     * Creating a Vulkan Instance with WSI extensions enabled
     */
    std::vector<char const *> desired_extensions;
    desired_extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
    desired_extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

    VkApplicationInfo application_info = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "MyApplicationName",
        VK_MAKE_VERSION(1, 0, 0),
        "MyEngineName",
        VK_MAKE_VERSION(1, 0, 0),
        VK_MAKE_VERSION(1, 0, 0) //
    };

    VkInstanceCreateInfo instance_create_info = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &application_info,
        0,       //static_cast<uint32_t>(desired_layers.size()),
        nullptr, //desired_layers.data(),
        static_cast<uint32_t>(desired_extensions.size()),
        desired_extensions.data() //
    };

    VkInstance instance;
    result = VulkanFunctions::vkCreateInstance(&instance_create_info, nullptr, &instance);
    if (result != VK_SUCCESS || instance == VK_NULL_HANDLE)
    {
        std::cout << "Create Vulkan Instance Failed." << std::endl;
        return 0;
    }

    if (!VulkanUtil::LoadInstanceLevelFunctions(instance, desired_extensions))
    {
        return 0;
    }

    /**
     * Creating a presentation surface
     * 
     * A presentation surface represents an application's window.
     * It allows us to acquire the window's parameters, such as dimensions, 
     * supported color formats, required number of images, or presentation modes. 
     * It also allows us to check whether a given physical device is
     * able to display an image in a given window.
     */

    HINSTANCE hInstance = GetModuleHandle(nullptr); // 参数为nullptr则返回该程序实例的handle

    const TCHAR CLASS_NAME[] = TEXT("Sample Window Class");

    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);
    // Create the window.
    HWND hWnd = CreateWindowEx(
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
    if (hWnd == NULL)
    {
        return 0;
    }

    VkWin32SurfaceCreateInfoKHR surface_create_info = {
        VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        nullptr,
        0,
        hInstance,
        hWnd //
    };

    VkSurfaceKHR presentation_surface = VK_NULL_HANDLE;
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)VulkanFunctions::vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
    result = vkCreateWin32SurfaceKHR(instance, &surface_create_info, nullptr, &presentation_surface);
    if (result != VK_SUCCESS || presentation_surface == VK_NULL_HANDLE)
    {
        std::cout << "Create Win32 Surface Failed!" << std::endl;
    }

    /**
     * Selecting a queue family that supports presentation to a given surface
     */
    /**
     * 先找一下有多少个device
     */
    uint32_t device_count;
    result = VulkanFunctions::vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    if (result != VK_SUCCESS || device_count == 0)
    {
        std::cout << "Find device Failed!" << std::endl;
        return 0;
    }
    /**
     * 把找到的device存起来
     */
    std::vector<VkPhysicalDevice> avaiable_devices(device_count);
    result = VulkanFunctions::vkEnumeratePhysicalDevices(instance, &device_count, avaiable_devices.data());
    if (result != VK_SUCCESS || device_count == 0)
    {
        std::cout << "Find device Failed!" << std::endl;
        return 0;
    }

    // 取第一个device，我这台电脑只有一个device
    VkPhysicalDevice physical_device = avaiable_devices[0];

    // 看一下device有多少个queue_family
    uint32_t queue_families_count;
    VulkanFunctions::vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);

    // 把找到的queue_family存起来
    std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
    VulkanFunctions::vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_families.data());

    /**
     * 找到一个支持presentation_surface的queue_family
     */
    uint32_t queue_family_index = queue_families.size();
    for (uint32_t index = 0; index < queue_families.size(); index++)
    {
        VkBool32 presentation_supported = VK_FALSE;
        result = VulkanFunctions::vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, index, presentation_surface, &presentation_supported);

        if (result == VK_SUCCESS && presentation_supported == VK_TRUE)
        {
            queue_family_index = index;
            break;
        }
    }

    if (queue_family_index == queue_families.size())
    {
        std::cout << "Can not find a queue_family that support surface extensions!" << std::endl;
        return 0;
    }

    /**
     * Creating a logical device with WSI extensions enabled
     */

    /**
     * 第五步，
     * 创建VkDeviceCreateInfo需要一个const char* const* ppEnabledExtensionNames;
     */
    std::vector<const char *> device_desired_extensions;
    device_desired_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    /**
     * 第四步，需要给queue_families中的queue赋个优先级的值。
     */
    std::vector<float> queue_priorities(queue_families[queue_family_index].queueCount);
    for (auto &value : queue_priorities)
    {
        value = 1.0f;
    }

    /**
     * 第三步
     * 创建VkDeviceCreateInfo需要一个VkDeviceQueueCreateInfo* pQueueCreateInfos
     */
    VkDeviceQueueCreateInfo queue_create_info = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        nullptr,
        0,
        queue_family_index,
        queue_families[queue_family_index].queueCount,
        queue_priorities.data() //
    };

    /**
     * 第二步，需要一个VkDeviceCreateInfo *pCreateInfo的东西嘛（可以单个，也可以是数组），创建一个
     * 不懂就看API
     */
    VkDeviceCreateInfo device_create_info = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr,
        0,
        1,
        &queue_create_info,
        0,
        nullptr,
        static_cast<uint32_t>(device_desired_extensions.size()),
        device_desired_extensions.data(),
        nullptr //
    };

    /**
     * 第一步，调用vkCreateDevice来创建logical device，然后看一下需要传什么参数
     */
    VkDevice logical_device;
    result = VulkanFunctions::vkCreateDevice(physical_device, &device_create_info, nullptr, &logical_device);
    if (result != VK_SUCCESS || logical_device == VK_NULL_HANDLE)
    {
        std::cout << "Create Logical Device Failed!" << std::endl;
        return 0;
    }

    /**
     * Load Devie level functions
     */
    std::vector<const char *> desired_enable_extensions;
    desired_enable_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    if (!VulkanUtil::LoadDeviceLevelFunctions(logical_device, desired_enable_extensions))
    {
        return 0;
    }

    /**
     *Selecting a desired presentation mode
     */
    uint32_t present_modes_cout;
    result = VulkanFunctions::vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, presentation_surface, &present_modes_cout, nullptr);
    if (result != VK_SUCCESS || present_modes_cout == 0)
    {
        std::cout << "Find present mode count failed!" << std::endl;
        return 0;
    }

    std::vector<VkPresentModeKHR> present_modes(present_modes_cout);
    result = VulkanFunctions::vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, presentation_surface, &present_modes_cout, present_modes.data());
    if (result != VK_SUCCESS || present_modes_cout == 0)
    {
        std::cout << "Find present mode count failed!" << std::endl;
        return 0;
    }

    std::cout << "\nSupported Present Mode: " << std::endl;
    for (auto &mode : present_modes)
    {
        std::cout << mode << std::endl;
    }

    VkPresentModeKHR desired_present_mode = VK_PRESENT_MODE_FIFO_KHR;

    /**
     * Getting the capabilities of a presentation surface
     */
    VkSurfaceCapabilitiesKHR surface_capabilities;
    result = VulkanFunctions::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, presentation_surface, &surface_capabilities);
    if (result != VK_SUCCESS)
    {
        std::cout << "Get Surface Capabilities Failed!" << std::endl;
        return 0;
    }

    /**
     * Selecting a number of swapchain images
     */
    uint32_t number_of_images = surface_capabilities.minImageCount + 1;
    number_of_images = min(number_of_images, surface_capabilities.maxImageCount);

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

    /**
     * Selecting a format of swapchain images
     */
    //VkFormat;
    //VkColorSpaceKHR;
    //由上面两个类型组成
    VkSurfaceFormatKHR desired_surface_format = {
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR //
    };

    uint32_t formats_count;
    result = VulkanFunctions::vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, presentation_surface, &formats_count, nullptr);
    if (result != VK_SUCCESS || formats_count == 0)
    {
        return 0;
    }

    std::vector<VkSurfaceFormatKHR> supported_surface_formats(formats_count);
    VulkanFunctions::vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, presentation_surface, &formats_count, supported_surface_formats.data());

    VkFormat format;
    VkColorSpaceKHR color_space;
    bool isSucc = false;

    if (supported_surface_formats[0].format == VK_FORMAT_UNDEFINED)
    {
        format = desired_surface_format.format;
        color_space = desired_surface_format.colorSpace;
    }
    else
    {
        for (auto &surface_format : supported_surface_formats)
        {
            if (surface_format.format == desired_surface_format.format && surface_format.colorSpace == desired_surface_format.colorSpace)
            {
                format = desired_surface_format.format;
                color_space = desired_surface_format.colorSpace;
                isSucc = true;
                break;
            }
        }
    }

    if (!isSucc)
    {
        format = supported_surface_formats[0].format;
        color_space = supported_surface_formats[0].colorSpace;
    }

    /**
     * Creating a swapchain
     */
    constexpr uint32_t image_count = 2;
    VkSwapchainKHR old_swapchain = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;

    VkSwapchainCreateInfoKHR swapchain_create_info = {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        nullptr,
        0,
        presentation_surface,
        image_count,
        format,
        color_space,
        size_of_images,
        1,
        desired_usage,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        nullptr,
        desired_transform,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        desired_present_mode,
        VK_TRUE,
        old_swapchain //
    };

    result = VulkanFunctions::vkCreateSwapchainKHR(logical_device, &swapchain_create_info, nullptr, &swapchain);
    if (result != VK_SUCCESS || swapchain == VK_NULL_HANDLE)
    {
        std::cout << "Create Swapchain Failed!" << std::endl;
        return 0;
    }

    if (old_swapchain != VK_NULL_HANDLE)
    {
        VulkanFunctions::vkDestroySwapchainKHR(logical_device, old_swapchain, nullptr);
    }

    /**
     * Acquiring a swapchain image
     * Before we can use a swapchain image, 
     * we need to ask a presentation engine for it. 
     * This process is called image acquisition.
     * 
     * Semaphores are used to synchronize device's queues.
     * but we can't use them to synchronize an application with the submitted command0s. 
     * To do so, we need to use fences. 
     */
    // VkSemaphore semaphore = VK_NULL_HANDLE;
    // VkFence fence = VK_NULL_HANDLE;
    uint32_t image_index;

    result = VulkanFunctions::vkAcquireNextImageKHR(logical_device, swapchain, static_cast<uint64_t>(2000000000), VK_NULL_HANDLE, VK_NULL_HANDLE, &image_index);
    if (result != VK_SUCCESS)
    {
        std::cout << "Acquire Iamge Failed!" << std::endl;
        return 0;
    }

    /**
     * Getting a device queua
     */
    /**
     * We can ask for a handle of the same queue multiple times. 
     * This call doesn't create queues--they are created implicitly
     * during logical device creation.
     */
    VkQueue queue;
    VulkanFunctions::vkGetDeviceQueue(logical_device, queue_family_index, 0, &queue);

    /**
     * Presenting an image
     */
    std::vector<VkSwapchainKHR> swapchains;
    std::vector<uint32_t> indices;
    swapchains.emplace_back(swapchain);
    indices.emplace_back(image_index);
    VkPresentInfoKHR present_info = {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        nullptr,
        0,
        nullptr,
        1,
        swapchains.data(),
        indices.data(),
        nullptr //
    };

    result = VulkanFunctions::vkQueuePresentKHR(queue, &present_info);
    if (result != VK_SUCCESS)
    {
        return 0;
    }

    /**
     * Creating a command pool
     */
    VkCommandPoolCreateInfo command_pool_create_info = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        nullptr,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        queue_family_index //
    };
    VkCommandPool command_pool;
    result = VulkanFunctions::vkCreateCommandPool(logical_device, &command_pool_create_info, nullptr, &command_pool);
    if (result != VK_SUCCESS)
    {
        std::cout << "Create Command Pool Failed!" << std::endl;
        return 0;
    }

    /**
     * Allocating command buffers
     */
    constexpr uint32_t command_buffer_count = 2;
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        nullptr,
        command_pool,
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        command_buffer_count //
    };
    std::vector<VkCommandBuffer> command_buffers(command_buffer_count);
    result = VulkanFunctions::vkAllocateCommandBuffers(logical_device, &command_buffer_allocate_info, command_buffers.data());
    if (result != VK_SUCCESS)
    {
        std::cout << "Allocate Command Buffers Failed!" << std::endl;
        return 0;
    }

    /**
     * Beginning a command buffer recording operation
     */
    VkCommandBuffer command_buffer = command_buffers[0];
    VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VkCommandBufferInheritanceInfo *secondary_command_buffer_info = nullptr;
    VkCommandBufferBeginInfo command_buffer_begin_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr,
        usage,
        secondary_command_buffer_info //
    };

    result = VulkanFunctions::vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
    if (result != VK_SUCCESS)
    {
        std::cout << "Begin Command Buffer Record Failed!" << std::endl;
        return 0;
    }

    /**
     * Creating a semaphore
     * Semaphores are used when we submit commands to queues.
     */
    VkSemaphoreCreateInfo semaphore_create_info = {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        nullptr,
        0 //
    };
    VkSemaphore semaphore;
    result = VulkanFunctions::vkCreateSemaphore(logical_device, &semaphore_create_info, nullptr, &semaphore);
    if (result != VK_SUCCESS)
    {
        std::cout << "Create Semaphore Failed!" << std::endl;
        return 0;
    }

    /**
     * Createing a fence
     * Fences, opposite to semaphores, 
     * are used to synchronize an application with commands submitted 
     * to the graphics hardware. 
     * They inform the application when the processing of a submitted 
     * work has been finished.
     */
    VkFenceCreateInfo fence_create_info = {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        nullptr,
        VK_FENCE_CREATE_SIGNALED_BIT //
    };
    VkFence fence;
    result = VulkanFunctions::vkCreateFence(logical_device, &fence_create_info, nullptr, &fence);
    if (result != VK_SUCCESS)
    {
        std::cout << "Create Fence Failed!" << std::endl;
        return 0;
    }

    /**
     * Waiting for fence
     */
    std::vector<VkFence> fences;
    fences.push_back(fence);
    VkBool32 wait_for_all = VK_TRUE;
    uint64_t timeout = 2000000000UL;
    result = VulkanFunctions::vkWaitForFences(logical_device, static_cast<uint32_t>(fences.size()), fences.data(), wait_for_all, timeout);

    /**
     * Resetting fences
     */
    result = VulkanFunctions::vkResetFences(logical_device, static_cast<uint32_t>(fences.size()), fences.data());
    if (result != VK_SUCCESS)
    {
        std::cout << "Reset Fences Failed!" << std::endl;
        return 0;
    }

    /**
     * Submitting command buffers to a queue
     */
    // TODO

    /**
     * Synchronizing two command buffers
     */
    // TODO

    /**
     * Checking if processing of a submitted command buffer has finished
     */
    // TODO

    /**
     * Waiting until all commands submitted to a queue are finished
     */
    // TODO

    /**
     * Waiting for all submitted commands to be finished
     */
    // TODO

    /**
     * Ending a command buffer recording operation
     */
    result = VulkanFunctions::vkEndCommandBuffer(command_buffer);
    if (result != VK_SUCCESS)
    {
        std::cout << "End Command Buffer Recored Failed!" << std::endl;
        return 0;
    }

    /**
     * Reseting a command buffer
     */
    VkCommandBufferResetFlags release_resource = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
    result = VulkanFunctions::vkResetCommandBuffer(command_buffer, release_resource);
    if (result != VK_SUCCESS)
    {
        std::cout << "Reset Command Buffer Failed!" << std::endl;
        return 0;
    }

    /**
     * Resetting a command pool
     */
    result = VulkanFunctions::vkResetCommandPool(logical_device, command_pool, release_resource);
    if (result != VK_SUCCESS)
    {
        std::cout << "Reset Command Pool Failed!" << std::endl;
        return 0;
    }

    /**
     * Destroying a fence
     */
    VulkanFunctions::vkDestroyFence(logical_device, fence, nullptr);
    fence = VK_NULL_HANDLE;

    /**
     * Destroying a semaphore
     */
    VulkanFunctions::vkDestroySemaphore(logical_device, semaphore, nullptr);
    semaphore = VK_NULL_HANDLE;

    /**
     * Freeing command buffers
     */
    VulkanFunctions::vkFreeCommandBuffers(logical_device, command_pool, static_cast<uint32_t>(command_buffers.size()), command_buffers.data());
    command_buffers.clear();

    /**
     * Destroying a command pool
     */
    VulkanFunctions::vkDestroyCommandPool(logical_device, command_pool, nullptr);
    command_pool = VK_NULL_HANDLE;

    /**
     * Destroying a swapchain
     */
    if (swapchain)
    {
        VulkanFunctions::vkDestroySwapchainKHR(logical_device, swapchain, nullptr);
        swapchain = VK_NULL_HANDLE;
    }

    /**
     * Destroying a presentation surface
     */
    if (presentation_surface)
    {
        VulkanFunctions::vkDestroySurfaceKHR(instance, presentation_surface, nullptr);
        presentation_surface = VK_NULL_HANDLE;
    }

    return 0;
}