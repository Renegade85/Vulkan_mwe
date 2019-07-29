#include <iostream>
#include <fstream>
#include <string>
#include "example.hpp"

static Vertex my_cube[] =
{
    /* front side */
    {VERTEX_1, COLOR_RED}, {VERTEX_2, COLOR_RED}, {VERTEX_3, COLOR_RED},
    {VERTEX_1, COLOR_RED}, {VERTEX_4, COLOR_RED}, {VERTEX_3, COLOR_RED},
    
    /* right side */
    {VERTEX_2, COLOR_GREEN}, {VERTEX_5, COLOR_GREEN}, {VERTEX_3, COLOR_GREEN},
    {VERTEX_5, COLOR_GREEN}, {VERTEX_7, COLOR_GREEN}, {VERTEX_3, COLOR_GREEN},
    
    /* left side */
    {VERTEX_1, COLOR_BLUE}, {VERTEX_6, COLOR_BLUE}, {VERTEX_8, COLOR_BLUE},
    {VERTEX_1, COLOR_BLUE}, {VERTEX_4, COLOR_BLUE}, {VERTEX_8, COLOR_BLUE},

    /* top side */
    {VERTEX_4, COLOR_RED}, {VERTEX_7, COLOR_RED}, {VERTEX_3, COLOR_RED},
    {VERTEX_8, COLOR_RED}, {VERTEX_4, COLOR_RED}, {VERTEX_7, COLOR_RED},
    
    /* bottom side */
    {VERTEX_1, COLOR_RED}, {VERTEX_2, COLOR_RED}, {VERTEX_5, COLOR_RED},
    {VERTEX_1, COLOR_RED}, {VERTEX_6, COLOR_RED}, {VERTEX_5, COLOR_RED},

    /* back side */
    {VERTEX_6, COLOR_RED}, {VERTEX_5, COLOR_RED}, {VERTEX_7, COLOR_RED},
    {VERTEX_6, COLOR_RED}, {VERTEX_8, COLOR_RED}, {VERTEX_7, COLOR_RED},
};

static void printResult(VkResult result, std::string message)
{
    switch (result)
    {
    case VK_SUCCESS:
        std::cout << message + ":[0] VK_SUCCESS"<< std::endl;
        break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        std::cout << message + ":[-7] VK_ERROR_EXTENSION_NOT_PRESENT" << std::endl;
        break;
    default:
        std::cout << result << std::endl;
        break;
    }
}

int32_t Example::createWindow(void)
{
    VkResult result;

    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

    if (!m_window)
    {
        std::cout << "Window creation failed!" << std::endl;
        return -1;
    }
    
    result = glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface);
    printResult(result, "Surface creation result");

    return 0;
}

void Example::run(void)
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        DrawFrame();
    }
}

void Example::createInstance(void)
{
    VkResult result;
    uint32_t extension_count;
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    if (VK_SUCCESS != result)
    {
        std::cout << "vkEnumerateInstanceExtensionProperties error. Unable to get extension count." << std::endl;
    }

    m_available_extensions.resize(extension_count);

    result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, &m_available_extensions[0]);
    if (VK_SUCCESS != result)
    {
        std::cout << "vkEnumerateInstanceExtensionProperties error. Unable to get extensions." << std::endl;
    }

    for (auto const& extension : m_available_extensions)
    {
        std::cout << extension.extensionName << std::endl;
    }

    uint32_t property_count;
    std::vector<VkLayerProperties> available_layers;
    result = vkEnumerateInstanceLayerProperties(&property_count, nullptr);
    printResult(result, "Enumerating layer properties");
    available_layers.resize(property_count);
    result = vkEnumerateInstanceLayerProperties(&property_count, &available_layers[0]);

    for (auto const& layer : available_layers)
    {
        std::cout << layer.layerName << std::endl;
    }

    VkInstanceCreateInfo ici = 
    {
        .sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                      = nullptr,
        .flags                      = 0,
        .pApplicationInfo           = nullptr,
        .enabledLayerCount          = (uint32_t) m_required_validation_layers.size(),
        .ppEnabledLayerNames        = &m_required_validation_layers[0],
        .enabledExtensionCount      = (uint32_t) m_required_instance_extensions.size(),
        .ppEnabledExtensionNames    = &m_required_instance_extensions[0],
    };

    result = vkCreateInstance(&ici, nullptr, &m_instance);

    printResult(result, "Instance creation result");   
}

void Example::createDevice(void)
{
    VkResult result;
    uint32_t best_device = 0u;

    uint32_t queue_count;
    std::vector<VkQueueFamilyProperties> queue_family_properties;

    uint32_t device_count;
    VkPhysicalDeviceProperties physical_device_properties;

    uint32_t extensionPropertiesCount;
    std::vector<VkExtensionProperties> deviceExtensionsProperties;

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {0u};
    physicalDeviceFeatures.depthClamp = VK_TRUE;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

    m_available_devices.resize(device_count);

    vkEnumeratePhysicalDevices(m_instance, &device_count, &m_available_devices[0]);

    /* select best device (now only based on type) */
    for (const auto& physical_device : m_available_devices)
    {
        vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
        std::cout << physical_device_properties.deviceName << std::endl;
        std::cout << physical_device_properties.deviceType << std::endl;
    }

    if (device_count > 1u)
    {
        m_selected_device = best_device;
    }
    else
    {
        m_selected_device = best_device;
    }

    vkEnumerateDeviceExtensionProperties(m_available_devices[m_selected_device], nullptr, &extensionPropertiesCount, nullptr);
    deviceExtensionsProperties.resize(extensionPropertiesCount);

    vkEnumerateDeviceExtensionProperties(m_available_devices[m_selected_device], nullptr, &extensionPropertiesCount, &deviceExtensionsProperties[0]);

    for (const auto & property : deviceExtensionsProperties)
    {
        std::cout << property.extensionName << ", version: " << property.specVersion << std::endl;
    }

    std::cout << "Getting queue family properties:" << std::endl;
    vkGetPhysicalDeviceQueueFamilyProperties(m_available_devices[m_selected_device], &queue_count, nullptr);

    std::cout << "Number of queues: " << queue_count << std::endl;
    queue_family_properties.resize(queue_count);

    vkGetPhysicalDeviceQueueFamilyProperties(m_available_devices[m_selected_device], &queue_count, &queue_family_properties[0]);

    

    /* select queue with graphics and present capabilities */
    uint8_t i = 0u;
    VkBool32 presentSupport;
    for (const auto& queue : queue_family_properties)
    {
        std::cout << "Flags: " << queue.queueFlags << std::endl;
        std::cout << "Queue count: " << queue.queueCount << std::endl << std::endl;

        if (0u != (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
            m_graphics_queue_idx = i;
        }

        vkGetPhysicalDeviceSurfaceSupportKHR(m_available_devices[m_selected_device], i, m_surface, &presentSupport);
        if (VK_TRUE == presentSupport)
        {
            m_present_queue_idx = i;
        }

        i++;
    }

    float queuePriorities[2u] = {1.f, 1.f};
    VkDeviceQueueCreateInfo qci =
        {
            .sType              = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext              = nullptr,
            .flags              = 0u,
            .queueFamilyIndex   = m_graphics_queue_idx,
            .queueCount         = 1u,
            .pQueuePriorities   = queuePriorities,
        };

    queueCreateInfos.push_back(qci);

    if (m_graphics_queue_idx != m_present_queue_idx)
    {
        qci.queueCount = 2u,
        qci.queueFamilyIndex = m_present_queue_idx;
        queueCreateInfos.push_back(qci);
    }

    VkDeviceCreateInfo dci = 
    {
        .sType                      = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                      = nullptr,
        .flags                      = 0,
        .queueCreateInfoCount       = (uint32_t) queueCreateInfos.size(),
        .pQueueCreateInfos          = &queueCreateInfos[0],
        .enabledLayerCount          = 0u,
        .ppEnabledLayerNames        = nullptr,
        .enabledExtensionCount      = (uint32_t) m_requiredPhysicalDeviceExtension.size(),
        .ppEnabledExtensionNames    = &m_requiredPhysicalDeviceExtension[0u],
        .pEnabledFeatures           = &physicalDeviceFeatures,
    };

    result = vkCreateDevice(m_available_devices[0], &dci, nullptr, &m_device);
    printResult(result, "Device creation result");
}

uint32_t Example::getQueueFamilyIndex()
{
    /* TODO update logic */
    return 0u;
}

void Example::createSwapchain(void)
{
    VkResult result;

    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_available_devices[m_selected_device], m_surface, &m_surfaceCapabilities);
    printResult(result, "Request for surface capabilities result");


    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_available_devices[m_selected_device], m_surface, &presentModeCount, nullptr);

    m_presentModes.resize(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_available_devices[m_selected_device], m_surface, &presentModeCount, &m_presentModes[0u]);
    printResult(result, "Request for surface present modes result");

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_available_devices[m_selected_device], m_surface, &formatCount, nullptr);

    m_surfaceFormats.resize(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_available_devices[m_selected_device], m_surface, &formatCount, &m_surfaceFormats[0u]);
    printResult(result, "Request for surface formats result");

    uint32_t imageCount = m_surfaceCapabilities.minImageCount;
    if ((m_surfaceCapabilities.maxImageCount > 1u) || (m_surfaceCapabilities.maxImageCount == 0u))
    {
        m_isDoubleBufferingSupported = VK_TRUE;
        m_selectedBufferingMode = DOUBLE_BUFFERING;
    }

    if ((m_surfaceCapabilities.maxImageCount > 2u) || (m_surfaceCapabilities.maxImageCount == 0u))
    {
        m_isTrippleBufferingSupported = VK_TRUE;
        //m_selectedBufferingMode = TRIPPLE_BUFFERING;
    }

    if (TRIPPLE_BUFFERING == m_selectedBufferingMode)
    {
        if (3u > imageCount) imageCount = 3u;
    }
    else if (DOUBLE_BUFFERING == m_selectedBufferingMode)
    {
        if (2u > imageCount) imageCount = 2u;
    }
    else
    {
        /* Neither Double buffering nor tripple buffering supported, handle error */
    }

    uint32_t queueFamilyIndices[1u] = {m_graphics_queue_idx};

    VkSwapchainCreateInfoKHR sci = 
    {
        .sType                  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                  = nullptr,
        .flags                  = 0,
        .surface                = m_surface,
        .minImageCount          = imageCount,
        .imageFormat            = m_surfaceFormats[3u].format,
        .imageColorSpace        = m_surfaceFormats[3u].colorSpace,
        .imageExtent            = m_surfaceCapabilities.currentExtent,
        .imageArrayLayers       = 1u,
        .imageUsage             = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode       = (m_graphics_queue_idx == m_present_queue_idx) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount  = 1u,
        .pQueueFamilyIndices    = queueFamilyIndices,
        .preTransform           = m_surfaceCapabilities.currentTransform,
        .compositeAlpha         = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode            = VK_PRESENT_MODE_FIFO_KHR,
        .clipped                = VK_TRUE,
        .oldSwapchain           = VK_NULL_HANDLE,
    };

    result = vkCreateSwapchainKHR(m_device, &sci, nullptr, &m_swapchain);
    printResult(result, "Swapchain creation result");

    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
    m_swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, &m_swapchainImages[0u]);
}

void Example::createImageViews(void)
{
    VkResult result;
    m_swapchainImageViews.resize(m_swapchainImages.size());
    
    VkImageViewCreateInfo ivci = 
    {
        .sType              = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = 0,
        .viewType           = VK_IMAGE_VIEW_TYPE_2D,
        .format             = m_surfaceFormats[3u].format,
        .components         = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange   = {VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u},
    };

    for (uint32_t i = 0u; i < m_swapchainImages.size(); i++)
    {
        ivci.image = m_swapchainImages[i];
        result = vkCreateImageView(m_device, &ivci, nullptr, &m_swapchainImageViews[i]);
        printResult(result, "Image view creation result");
    }
}

void Example::createDepthResources(void)
{
    VkResult result;
    
    /* TODO Which format is the best? How to guarantee functionality */
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_available_devices[m_selected_device], VK_FORMAT_D32_SFLOAT, &formatProperties);

    std::cout << std::hex << formatProperties.optimalTilingFeatures << std::endl;
    std::cout << std::hex << formatProperties.linearTilingFeatures  << std::endl;

    VkImageCreateInfo imageInfo =
    {
        .sType                  = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .imageType              = VK_IMAGE_TYPE_2D,
        .format                 = VK_FORMAT_D32_SFLOAT,
        .extent                 = {640, 480, 1},
        .mipLevels              = 1u,
        .arrayLayers            = 1u,
        .samples                = VK_SAMPLE_COUNT_1_BIT,
        .tiling                 = VK_IMAGE_TILING_OPTIMAL,
        .usage                  = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .sharingMode            = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout          = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    result = vkCreateImage(m_device, &imageInfo, nullptr, &m_depthImage);
    printResult(result, "Depth image creation result");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, m_depthImage, &memRequirements);

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_available_devices[m_selected_device], &memProperties);

    VkMemoryAllocateInfo mai = 
    {
        .sType              = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext              = nullptr,
        .allocationSize     = memRequirements.size,
        .memoryTypeIndex    = 0u,
    };

    vkAllocateMemory(m_device, &mai, nullptr, &m_depthImageMemory);
    vkBindImageMemory(m_device, m_depthImage, m_depthImageMemory, 0u);
    VkImageViewCreateInfo ivci =
    {
        .sType              = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = 0,
        .image              = m_depthImage,
        .viewType           = VK_IMAGE_VIEW_TYPE_2D,
        .format             = VK_FORMAT_D32_SFLOAT,
        .components         = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange   = {VK_IMAGE_ASPECT_DEPTH_BIT, 0u, 1u, 0u, 1u},
    };
    result = vkCreateImageView(m_device, &ivci, nullptr, &m_depthImageView);
    printResult(result, "Depth buffer image view creation result");
}

void Example::createRenderPass(void)
{
    VkResult result;
    VkAttachmentDescription attachment_descriptions[] = 
    {
        /* Color attachment */
        {
            .flags          = 0,
            .format         = VK_FORMAT_R8G8B8A8_SRGB,
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        },
        /* Depth attachment */
        {
            .flags          = 0,
            .format         = VK_FORMAT_D32_SFLOAT,
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        }
    };

    VkAttachmentReference color_attachment_references[] = 
    {
        {
            .attachment = 0u,
            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        }
    };

    VkAttachmentReference depth_attachment_references[] =
    {
        {
            .attachment = 1u,
            .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        }
    };

    VkSubpassDescription sds[] = 
    {
        {
            .flags                      = 0,
            .pipelineBindPoint          = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount       = 0u,
            .pInputAttachments          = nullptr,
            .colorAttachmentCount       = 1u,
            .pColorAttachments          = color_attachment_references,
            .pResolveAttachments        = nullptr,
            .pDepthStencilAttachment    = depth_attachment_references,
            .preserveAttachmentCount    = 0u,
            .pPreserveAttachments       = nullptr,
        }
    };

    VkRenderPassCreateInfo rpci = 
    {
        .sType              = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = 0,
        .attachmentCount    = 2u,
        .pAttachments       = attachment_descriptions,
        .subpassCount       = 1u,
        .pSubpasses         = sds,
        .dependencyCount    = 0u,
        .pDependencies      = nullptr,
    };

    result = vkCreateRenderPass(m_device, &rpci, nullptr, &m_renderPass);
    printResult(result, "Renderpass creation result");
}

void Example::createFramebuffers(void)
{
    VkResult result;
    VkFramebufferCreateInfo fci =
    {
        .sType              = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = 0,
        .renderPass         = m_renderPass,
        .attachmentCount    = 2u,
        .pAttachments       = nullptr,
        .width              = m_surfaceCapabilities.currentExtent.width,
        .height             = m_surfaceCapabilities.currentExtent.height,
        .layers             = 1u,
    };

    m_framebuffers.resize(m_swapchainImageViews.size());

    for (uint8_t i = 0; i < m_swapchainImageViews.size(); i++)
    {
        VkImageView attachments[] = {m_swapchainImageViews[i], m_depthImageView};
        fci.pAttachments = attachments;

        result = vkCreateFramebuffer(m_device, &fci, nullptr, &m_framebuffers[i]);
        printResult(result, "Framebuffer creation result");
    }
}

std::vector<char> readFile(const std::string & pathToShader)
{
    std::ifstream file(pathToShader, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

void Example::createPipeline(void)
{
    /*
    vkCreatePipelineLayout -> VkPipelineLayout
    |-> VkPipelineLayoutCreateInfo
        |-> VkDescriptorSetLayout

    vkCreateDescriptorSetLayout -> VkDescriptorSetLayout
    |-> VkDescriptorSetLayoutCreateInfo
        |-> VkDescriptorSetLayoutBindings
    
    vkAllocateDescriptorSets -> VkDescriptorSet
    |-> VkDescriptorSetAllocateInfo
        |-> VkDescriptorSetLayout
    */
    /* Get memory requirements for model */
    VkResult result;
    uint32_t queueFamilyIndices = {0};
    VkBufferCreateInfo bci = 
    {
        .sType                  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .size                   = sizeof(my_cube),
        .usage                  = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode            = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount  = 1,
        .pQueueFamilyIndices    = &queueFamilyIndices,
    };
    result = vkCreateBuffer(m_device, &bci, nullptr, &m_modelBuffer);
    printResult(result, "Buffer creation result");

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(m_device, m_modelBuffer, &memoryRequirements);

    VkMemoryAllocateInfo mai = 
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = 0,
    };
    result = vkAllocateMemory(m_device, &mai, nullptr, &m_modelBufferMemory);
    printResult(result, "Memory allocation for buffer result");

    result = vkBindBufferMemory(m_device, m_modelBuffer, m_modelBufferMemory, 0u);
    printResult(result, "Binding memory result");

    /* Load model data into memory */
    void * data;
    result = vkMapMemory(m_device, m_modelBufferMemory, 0, memoryRequirements.size, 0, &data);
    printResult(result, "Mapping memory result");

    memcpy(data, &my_cube[0], bci.size);
    vkUnmapMemory(m_device, m_modelBufferMemory);


    VkPipelineInputAssemblyStateCreateInfo piasci = 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE, /* TODO ask about restart enable, how does it work? */
    };

    VkVertexInputBindingDescription vibds[] =
    {
        {
            .binding = 0u,
            .stride = sizeof(struct Vertex),   /* size of vertex */
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        }
    };

    VkVertexInputAttributeDescription viads[] = 
    {
        { .location = 0u, .binding = 0u, .format = VK_FORMAT_R32G32B32A32_SFLOAT, .offset = offsetof(Vertex, coord)},
        { .location = 1u, .binding = 0u, .format = VK_FORMAT_R32G32B32A32_SFLOAT, .offset = offsetof(Vertex, color)}
    };

    VkPipelineVertexInputStateCreateInfo pvisci = 
    {
        .sType                              = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                              = nullptr,
        .flags                              = 0,
        .vertexBindingDescriptionCount      = sizeof(vibds) / sizeof(vibds[0]),
        .pVertexBindingDescriptions         = vibds,
        .vertexAttributeDescriptionCount    = sizeof(viads) / sizeof(viads[0]),
        .pVertexAttributeDescriptions       = viads,
    };

    std::vector<char> vertexShaderCode = readFile("./shaders/vert.spv");
    std::vector<char> fragmentShaderCode = readFile("./shaders/frag.spv");

    VkShaderModuleCreateInfo vertexShaderCreateInfo =
    {
        .sType      = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext      = nullptr,
        .flags      = 0,
        .codeSize   = vertexShaderCode.size(),
        .pCode      = reinterpret_cast<const uint32_t *>(vertexShaderCode.data()),
    };

    VkShaderModule vertexShaderModule;
    result = vkCreateShaderModule(m_device, &vertexShaderCreateInfo, nullptr, &vertexShaderModule);
    printResult(result, "Vertex shader module module creation status");
    
    VkViewport vp[] = 
    {
        {.x = 0.0f, .y = 0.0f, .width = 640.0f, .height = 480.0f, .minDepth = 0.f, .maxDepth = 1.f}
    };

    VkRect2D scissors[] = 
    {
        {.offset = {0, 0}, .extent = {640u, 480u}}
    };

    VkPipelineViewportStateCreateInfo pvsci = 
    {
        .sType          = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext          = nullptr,
        .flags          = 0,
        .viewportCount  = 1u,
        .pViewports     = vp,
        .scissorCount   = 1u,
        .pScissors      = scissors,
    };

    VkPipelineRasterizationStateCreateInfo prsci = 
    {
        .sType                      = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                      = nullptr,
        .flags                      = 0,
        .depthClampEnable           = VK_FALSE,//VK_TRUE,
        .rasterizerDiscardEnable    = VK_FALSE,//VK_TRUE,
        .polygonMode                = VK_POLYGON_MODE_FILL,
        .cullMode                   = VK_CULL_MODE_BACK_BIT,
        .frontFace                  = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable            = VK_FALSE,
        .depthBiasConstantFactor    = 0.0f,
        .depthBiasClamp             = 0.0f,
        .depthBiasSlopeFactor       = 0.0f,
        .lineWidth                  = 1.0f,
    };

    VkShaderModuleCreateInfo fragmentShaderCreateInfo =
    {
        .sType      = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext      = nullptr,
        .flags      = 0,
        .codeSize   = fragmentShaderCode.size(),
        .pCode      = reinterpret_cast<const uint32_t *>(fragmentShaderCode.data()),
    };

    VkShaderModule fragmentShaderModule;
    result = vkCreateShaderModule(m_device, &fragmentShaderCreateInfo, nullptr, &fragmentShaderModule);
    printResult(result, "Fragment shader module creation result");

    VkPipelineShaderStageCreateInfo shaderStagesInfo[] = 
    {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertexShaderModule,
            .pName = "main",
            .pSpecializationInfo = nullptr,
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragmentShaderModule,
            .pName = "main",
            .pSpecializationInfo = nullptr,
        }
    };

    /* TODO pipeline layout??? */
    VkPipelineLayoutCreateInfo plci = 
    {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = 0u,
        .pSetLayouts            = nullptr,
        .pushConstantRangeCount = 0u,
        .pPushConstantRanges    = nullptr,
    };

    result = vkCreatePipelineLayout(m_device, &plci, nullptr, &m_pipelineLayout);
    printResult(result, "Pipeline layout creation result");

    /* Test */
    VkPipelineMultisampleStateCreateInfo pmssci = 
    {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .rasterizationSamples   = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable    = VK_FALSE,
        .minSampleShading       = 1.f,
        .pSampleMask            = nullptr,
        .alphaToCoverageEnable  = VK_FALSE,
        .alphaToOneEnable       = VK_FALSE,
    };

    VkPipelineDepthStencilStateCreateInfo pdssci = 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkPipelineColorBlendAttachmentState pcbas = 
    {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo pcbsci = 
    {
        .sType              = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = 0,
        .logicOpEnable      = VK_FALSE,
        .logicOp            = VK_LOGIC_OP_CLEAR,
        .attachmentCount    = 1,
        .pAttachments       = &pcbas,
        .blendConstants     = {0.f, 0.f, 0.f, 0.f},
    };

    VkGraphicsPipelineCreateInfo ci =
    {
        .sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .stageCount             = 2u,
        .pStages                = shaderStagesInfo,
        .pVertexInputState      = &pvisci,
        .pInputAssemblyState    = &piasci,
        .pTessellationState     = nullptr,
        .pViewportState         = &pvsci,
        .pRasterizationState    = &prsci,
        .pMultisampleState      = &pmssci,
        .pDepthStencilState     = &pdssci,
        .pColorBlendState       = &pcbsci,
        .pDynamicState          = nullptr,
        .layout                 = m_pipelineLayout,
        .renderPass             = m_renderPass,
        .subpass                = 0u,
        .basePipelineHandle     = VK_NULL_HANDLE,
        .basePipelineIndex      = -1,
    };

    m_pipelines.resize(1u);
    result = vkCreateGraphicsPipelines(m_device,
                                       VK_NULL_HANDLE,
                                       1,
                                       &ci,
                                       nullptr,
                                       &m_pipelines[0]);
    printResult(result, "Graphics pipeline creation result");

    vkDestroyShaderModule(m_device, vertexShaderModule, nullptr);
    vkDestroyShaderModule(m_device, fragmentShaderModule, nullptr);
}

void Example::createCommandBuffers(void)
{
    VkResult result;

    VkCommandPoolCreateInfo cpci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = 0u,
    };

    result = vkCreateCommandPool(m_device, &cpci, NULL, &m_commandPool);
    printResult(result, "Command pool creation result");

    m_commandBuffers.resize(m_framebuffers.size());

    VkCommandBufferAllocateInfo cbai = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = NULL,
        .commandPool        = m_commandPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t) m_commandBuffers.size(),
    };
    result = vkAllocateCommandBuffers(m_device, &cbai, m_commandBuffers.data());
    printResult(result, "Command buffer allication result");

    VkCommandBufferBeginInfo cbbi = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    VkClearValue clearValues[] = {0.f, 1.f};
    VkRenderPassBeginInfo rpbi = {
        .sType          = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext          = NULL,
        .renderPass     = m_renderPass,
        .renderArea     = {{0, 0}, {640u, 480u}},
        .clearValueCount = 2u,
        .pClearValues   = clearValues,
    };

    VkDeviceSize offsets[] = {0u};
    for (uint8_t i = 0u; i < m_commandBuffers.size(); i++)
    {
        result = vkBeginCommandBuffer(m_commandBuffers[i], &cbbi);
        rpbi.framebuffer = m_framebuffers[i];

        vkCmdBeginRenderPass(m_commandBuffers[i], &rpbi, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[0u]);
        vkCmdBindVertexBuffers(m_commandBuffers[i], 0, 1, &m_modelBuffer, offsets);
        vkCmdDraw(m_commandBuffers[i], 36u, 1u, 0u, 0u);

        result = vkEndCommandBuffer(m_commandBuffers[i]);
    }
}

void Example::createSemaphores(void)
{
    VkSemaphoreCreateInfo sci =
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    
    m_imageReadySemaphores.resize(m_swapchainImages.size());
    for (uint32_t i = 0u; i < m_swapchainImages.size(); i++)
    {
        vkCreateSemaphore(m_device, &sci, nullptr, &m_imageReadySemaphores[i]);
    }

    vkCreateSemaphore(m_device, &sci, nullptr, &m_renderDoneSemaphore);
}

void Example::createFences(void)
{
    VkFenceCreateInfo fci = 
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    m_drawFences.resize(m_maxInflightSubmissions);
    for (uint32_t i = 0u; i < m_maxInflightSubmissions; i++)
    {
        vkCreateFence(m_device, &fci, nullptr, &m_drawFences[i]);
    }
}

void Example::DrawFrame(void)
{
    VkResult result;
    uint32_t nextImageIndex;
    VkPresentInfoKHR presentInfo;

    vkWaitForFences(m_device, 1, &m_drawFences[m_submissionNumber], VK_TRUE, UINT64_MAX);
    vkResetFences(m_device, 1, &m_drawFences[m_submissionNumber]);

    result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_imageReadySemaphores[m_submissionNumber], VK_NULL_HANDLE, &nextImageIndex);

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    /* Use sync primitive so we don't modify image being read from */
    if (VK_SUCCESS == result)
    {
        /* Queue all rendering commands and transition the image layout  */
        VkSubmitInfo submitInfo = {
            .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext              = NULL,
            .waitSemaphoreCount = 1u,
            .pWaitSemaphores    = &m_imageReadySemaphores[m_submissionNumber],
            .pWaitDstStageMask  = &pipelineStageFlags,
            .commandBufferCount = 1u,
            .pCommandBuffers    = &m_commandBuffers[nextImageIndex],
            .signalSemaphoreCount = 1u,
            .pSignalSemaphores  = &m_renderDoneSemaphore,
        };
        VkQueue graphicsQueue;
        vkGetDeviceQueue(m_device, 0u, m_graphics_queue_idx, &graphicsQueue);
        vkQueueSubmit(graphicsQueue, 1u, &submitInfo, m_drawFences[m_submissionNumber]);

        m_submissionNumber = (m_submissionNumber + 1u) % m_maxInflightSubmissions;

        /* Queue the image for presentation */
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = NULL;
        presentInfo.waitSemaphoreCount = 1u;
        presentInfo.pWaitSemaphores = &m_renderDoneSemaphore;
        presentInfo.swapchainCount = 1u;
        presentInfo.pSwapchains = &m_swapchain;
        presentInfo.pImageIndices = &nextImageIndex;
        presentInfo.pResults = NULL;

        VkQueue presentQueue;
        vkGetDeviceQueue(m_device, 0u, m_present_queue_idx, &presentQueue);
        result = vkQueuePresentKHR(presentQueue, &presentInfo);
    }
}

void Example::cleanup(void)
{
    /* Command pool may be freed only after all command buffers are in pending???/init state??? */
    //vkDestroyCommandPool(m_device, m_commandPool, NULL);
    vkDestroyBuffer(m_device, m_modelBuffer, nullptr);
    vkFreeMemory(m_device, m_modelBufferMemory, nullptr);

    vkDestroyImageView(m_device, m_depthImageView, nullptr);
    vkDestroyImage(m_device, m_depthImage, nullptr);
    vkFreeMemory(m_device, m_depthImageMemory, nullptr);
    for (auto & framebuffer : m_framebuffers)
    {
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    for (auto imageView : m_swapchainImageViews)
    {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    for (auto & pipeline : m_pipelines)
    {
        vkDestroyPipeline(m_device, pipeline, nullptr);
    }
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();
    vkDestroyInstance(m_instance, nullptr);
}