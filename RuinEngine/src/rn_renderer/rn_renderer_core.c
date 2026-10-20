#include "rn_platform/rn_platform.h"
#include "rn_renderer.h"
#include "ruin.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_video.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan_core.h>

static const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};

static const char *device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
};

static const size_t validation_layer_count = 1;



static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    printf("\nvalidation layer: %s\n\n", pCallbackData->pMessage);

    return VK_FALSE;
}



uint8_t check_validation_layer_support() {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    VkLayerProperties available_layers[layer_count];
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for (size_t i = 0; i < validation_layer_count; i++) {
        uint8_t layer_found = 0;

        const char *layer_name = validation_layers[i];

        for (size_t j = 0; j < layer_count; j++) {
            VkLayerProperties *layer_properties = &(available_layers[j]);

            if (strcmp(layer_name, layer_properties->layerName) == 0) {
                layer_found = 1;
                break;
            }
        }

        if (!layer_found) {
            return 0;
        }
    }

    return 1;
}


void rn_renderer_create_instance(RN_Renderer *r, RN_Platform *p) {

    if (volkInitialize() != VK_SUCCESS) {
        RUIN_DEBUG("Vulkan Library not found.\n");
        return;
    }
    
    uint32_t extension_count = 0;
    const char * const *sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&extension_count);

    uint8_t debug_extension = 0;

    if (enable_validation) {
        debug_extension = 1;
    }

    const char *required_extensions[extension_count + 3];

    for (size_t i = 0; i < extension_count; i++) {
        required_extensions[i] = sdl_extensions[i];
    }

    if (enable_validation) {
        required_extensions[extension_count] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        extension_count += 1;
    }
    

    VkApplicationInfo ai  = {0};
    ai.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pApplicationName   = p->config.title;
    ai.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    ai.pEngineName        = "Ruin Engine";
    ai.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    ai.apiVersion         = VK_API_VERSION_1_3;
    ai.pNext              = NULL;

    VkInstanceCreateInfo ii    = {0};
    ii.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ii.pApplicationInfo        = &ai;
    ii.ppEnabledExtensionNames = required_extensions;
    ii.enabledExtensionCount   = extension_count;
    ii.pNext                   = NULL;

    if (enable_validation) {
        if (!check_validation_layer_support()) {
            RUIN_DEBUG("Validation layers requested but not available.\n");
            return;
        }

        ii.ppEnabledLayerNames = validation_layers;
        ii.enabledLayerCount   = validation_layer_count;
    }
    
    VkResult res = vkCreateInstance(&ii, NULL, &r->core.instance);
    if (res != VK_SUCCESS) {
        RUIN_DEBUG("Failed to create instance: %d\n", res);
        return;
    }

    RUIN_DEBUG("Instance created.\n");

    volkLoadInstance(r->core.instance);

    
    return;
}






void rn_renderer_create_debug_messenger(RN_Renderer *r) {

    if (!check_validation_layer_support()) {
        RUIN_DEBUG("Validation layers not found.\n");
        return;
    }
    
    PFN_vkCreateDebugUtilsMessengerEXT fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(r->core.instance, "vkCreateDebugUtilsMessengerEXT");

    VkDebugUtilsMessengerCreateInfoEXT mi = {0};
    mi.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    
    mi.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT   |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    
    mi.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

    mi.pfnUserCallback = debugCallback;
    

    if (fn != NULL) {
        if (fn(r->core.instance, &mi, NULL, &r->core.messenger) != VK_SUCCESS) {
            RUIN_DEBUG("Failed to create debug messenger.\n");
            return;
        }
    }

    RUIN_DEBUG("Debug messenger created.\n");
}



void rn_renderer_create_surface(RN_Renderer *r, RN_Platform *p) {
    if (SDL_Vulkan_CreateSurface((SDL_Window*)p->window.window, r->core.instance, NULL, &r->core.surface) == 0) {
        RUIN_DEBUG("Failed to create Surface KHR.\n");
        printf("%s\n", SDL_GetError());
        return;
    }

    RUIN_DEBUG("Surface KHR created.\n");
}



void rn_renderer_select_physical_device(RN_Renderer *r) {

    uint32_t device_count;
    vkEnumeratePhysicalDevices(r->core.instance, &device_count, NULL);

    if (device_count == 0) {
        RUIN_DEBUG("Your PC is trash.\n");        
        return;
    }

    RUIN_DEBUG("You have %d physical devices.\n", device_count);

    VkPhysicalDevice p_devices[device_count];
    vkEnumeratePhysicalDevices(r->core.instance, &device_count, p_devices);

    for (size_t i = 0; i < device_count; i++) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(p_devices[i], &props);

        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            r->core.physical_device = p_devices[i];
            
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(r->core.physical_device, &props);
            RUIN_DEBUG("Selected physical device: %s\n", props.deviceName);
            return;
        }
    }

    for (size_t i = 0; i < device_count; i++) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(p_devices[i], &props);

        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            r->core.physical_device = p_devices[i];

            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(r->core.physical_device, &props);
            RUIN_DEBUG("Selected physical device: %s\n", props.deviceName);
            return;
        }
    }

    r->core.physical_device = p_devices[0];
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(r->core.physical_device, &props);

    RUIN_DEBUG("Selected physical device: %s\n", props.deviceName);
}

void rn_renderer_create_logical_device(RN_Renderer *r) {
    QueueFamilyIndices indices = find_queue_family_indices(&r->core);

    r->core.queue_families = indices;

    uint32_t q_count = (indices.graphics_family == indices.present_family) ? 1 : 2;
    float queue_priority = 1.0f;

    VkDeviceQueueCreateInfo qis[2];
    uint32_t qfs[2] = { indices.graphics_family, indices.present_family };

    for (size_t i = 0; i < q_count; i++) {
        qis[i].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qis[i].queueFamilyIndex = qfs[i];
        qis[i].queueCount       = 1;
        qis[i].pQueuePriorities = &queue_priority;
        qis[i].pNext            = NULL;
        qis[i].flags            = 0;
    }

    VkPhysicalDeviceFeatures f = {};
    f.samplerAnisotropy = VK_TRUE;

    VkPhysicalDeviceVulkan12Features f12 = {0};
    f12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    f12.descriptorBindingPartiallyBound               = VK_TRUE;
    f12.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
    f12.runtimeDescriptorArray                        = VK_TRUE;
    f12.shaderStorageBufferArrayNonUniformIndexing    = VK_TRUE;
    f12.bufferDeviceAddress                           = VK_TRUE;
    f12.shaderSampledImageArrayNonUniformIndexing     = VK_TRUE;
    f12.descriptorBindingSampledImageUpdateAfterBind  = VK_TRUE;
    f12.descriptorBindingVariableDescriptorCount      = VK_TRUE;

    VkPhysicalDeviceVulkan13Features f13 = {0};
    f13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    f13.pNext = &f12;
    f13.dynamicRendering = VK_TRUE;
    f13.synchronization2 = VK_TRUE;

    VkPhysicalDeviceFeatures2 features2 = {0};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.features = f;
    features2.pNext = &f13;

    VkDeviceCreateInfo di = {0};
    di.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    di.pQueueCreateInfos       = qis;
    di.queueCreateInfoCount    = q_count;
    di.pEnabledFeatures        = nullptr;
    di.ppEnabledExtensionNames = device_extensions;
    di.enabledExtensionCount   = 4;
    di.pNext                   = &features2;

    if (vkCreateDevice(r->core.physical_device, &di, NULL, &r->core.device) != VK_SUCCESS) {
        RUIN_DEBUG("Failed to create Logical Device.\n");
        return;
    }

    RUIN_DEBUG("Logical device created.\n");


    volkLoadDevice(r->core.device);
    

    vkGetDeviceQueue(r->core.device, indices.graphics_family, 0, &r->core.graphics_queue);
    RUIN_DEBUG("Graphics queue acquired.\n");

    if (q_count == 2) {
        vkGetDeviceQueue(r->core.device, indices.present_family, 0, &r->core.present_queue);
        RUIN_DEBUG("Presentation queue acquired.\n");
        return;
    }

    r->core.present_queue = r->core.graphics_queue;
    RUIN_DEBUG("Graphics queue will be used for presentation.\n");
}







