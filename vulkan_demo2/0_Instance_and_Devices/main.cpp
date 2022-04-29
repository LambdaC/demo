#include <iostream>
#include <vector>
#include <bitset>
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
     * crate Vulkan instance
     */
    std::vector<char const *> desired_extensions;

    for (auto &extensions : available_extensions)
    {
        desired_extensions.push_back(extensions.extensionName);
    }

    std::vector<char const *> desired_layers;
    for (auto &layers : available_layers)
    {
        desired_layers.push_back(layers.layerName);
    }

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

    /**
     * Load Instacne Level functions
     * Instance-level functions are used for manipulating with physical
     * devices, checking their properties, abilities and, creating logical devices.
     */
    if (!VulkanUtil::LoadInstanceLevelFunctions(instance, desired_extensions))
    {
        return 0;
    }

    /**
     * Enumerating available physical devices
     */
    uint32_t devices_count;
    result = VulkanFunctions::vkEnumeratePhysicalDevices(instance, &devices_count, nullptr);
    if (result != VK_SUCCESS || devices_count == 0)
    {
        std::cout << "Could not find a devices that Support Vulkan";
        return 0;
    }

    std::cout << "\nDevices Count: " << devices_count << std::endl;

    std::vector<VkPhysicalDevice> available_devices(devices_count);
    result = VulkanFunctions::vkEnumeratePhysicalDevices(instance, &devices_count, available_devices.data());

    /**
     * check available device extensions
     */
    VkPhysicalDevice physical_device = available_devices[0];
    result = VulkanFunctions::vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, nullptr);
    if (result != VK_SUCCESS || extensions_count == 0)
    {
        std::cout << "Check available device extensions Failed" << std::endl;
        return 0;
    }

    std::cout << "\nDevice Extensions Count: " << extensions_count << std::endl;

    std::vector<VkExtensionProperties> device_available_extensions(extensions_count);
    result = VulkanFunctions::vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, device_available_extensions.data());

    for (auto &extension : device_available_extensions)
    {
        std::cout << extension.extensionName << std::endl;
    }

    /**
     * Getting features and properties of a physical device
     */
    VkPhysicalDeviceProperties device_properties;
    VulkanFunctions::vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    std::cout << "\nDevice Name: " << device_properties.deviceName << std::endl;

    VkPhysicalDeviceFeatures device_features;
    VulkanFunctions::vkGetPhysicalDeviceFeatures(physical_device, &device_features);

    VkPhysicalDeviceFeatures *pDevice_features = &device_features;
    void *pVoid = pDevice_features;
    std::cout << "\nEnable Features:" << std::endl;
    VkBool32 *pBool32 = static_cast<VkBool32 *>(pVoid);
    for (int i = 0; i < 55; i++)
    {
        std::cout << i << ": " << *pBool32 << std::endl;
    }

    /**
     * Checking available queue families and their properties
     */
    uint32_t queue_families_count;
    VulkanFunctions::vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);
    std::cout << "\nQueue Families Count: " << queue_families_count << std::endl;

    std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
    VulkanFunctions::vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_families.data());

    for (auto &family : queue_families)
    {
        std::cout << "Family Queue Flags: " << std::bitset<8>(family.queueFlags) << std::endl;
        std::cout << "This Family Queue Count: " << family.queueCount << std::endl;
    }

    /**
     * Selecting the index of a queue family with the desired capabilities
     */
    uint32_t queue_family_index = queue_families.size();
    VkQueueFlags desired_capabilities = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_SPARSE_BINDING_BIT | VK_QUEUE_TRANSFER_BIT;
    for (uint32_t index = 0; index < queue_families_count; index++)
    {
        if (queue_families[index].queueCount > 0 && (queue_families[index].queueFlags & desired_capabilities))
        {
            queue_family_index = index;
        }
    }
    if (queue_family_index == queue_families.size())
    {
        std::cout << "\nCould not find a suitable queue." << std::endl;
        return 0;
    }

    /**
     * Creating a logical device
     */
    std::vector<float> queue_priorities(queue_families[queue_family_index].queueCount);
    for (auto &number : queue_priorities)
    {
        number = 0.5f;
    }

    VkDeviceQueueCreateInfo queue_create_info = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        nullptr,
        0,
        queue_family_index,
        queue_families[queue_family_index].queueCount,
        queue_priorities.data(), //
    };

    // std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

    // for (int i = 0; i < queue_families.size(); i++)
    // {
    //     auto queue_family = queue_families[i];
    //     std::vector<float> queue_pri(queue_family.queueCount);
    //     for (auto &number : queue_pri)
    //     {
    //         number = 0.5f;
    //     }

    //     VkDeviceQueueCreateInfo queue_info = {
    //         VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    //         nullptr,
    //         0,
    //         i,
    //         queue_family.queueCount,
    //         queue_pri.data(), //
    //     };
    //     queue_create_infos.push_back(queue_info);
    // }

    std::vector<const char *> device_desired_extensions;
    for (auto &extension : device_available_extensions)
    {
        device_desired_extensions.push_back(extension.extensionName);
    }
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
        &device_features //
    };
    VkDevice logical_device;
    result = VulkanFunctions::vkCreateDevice(physical_device, &device_create_info, nullptr, &logical_device);
    if (result != VK_SUCCESS)
    {
        std::cout << "\nCreate Logical Device Failed." << std::endl;
        return 0;
    }

    /**
     * Loading device-level functions
     */
    if (!VulkanUtil::LoadDeviceLevelFunctions(logical_device, device_desired_extensions))
    {
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
     * Creating a logical device with geometry shaders, graphics, and compute queues
     */
    // 应用上面所有的例子获取特定的Queue

    /**
     * Destroying a logical device
     */
    if (logical_device)
    {
        VulkanFunctions::vkDestroyDevice(logical_device, nullptr);
        logical_device = VK_NULL_HANDLE;
    }

    /**
     * Destroying a Vulkan Instance
     */
    if (instance)
    {
        VulkanFunctions::vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }

    /**
     * Releasing a Vulkan Loader library
     */
    if (VulkanUtil::vulkan_library)
    {
        FreeLibrary(VulkanUtil::vulkan_library);
        VulkanUtil::vulkan_library = nullptr;
    }

    return 0;
}