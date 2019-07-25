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
    {VERTEX_2, COLOR_RED}, {VERTEX_5, COLOR_RED}, {VERTEX_3, COLOR_RED},
    {VERTEX_5, COLOR_RED}, {VERTEX_7, COLOR_RED}, {VERTEX_3, COLOR_RED},
    
    /* left side */
    {VERTEX_1, COLOR_RED}, {VERTEX_6, COLOR_RED}, {VERTEX_8, COLOR_RED},
    {VERTEX_1, COLOR_RED}, {VERTEX_4, COLOR_RED}, {VERTEX_8, COLOR_RED},

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
        m_selectedBufferingMode = TRIPPLE_BUFFERING;
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

void Example::createRenderPass(void)
{
    VkResult result;
    VkAttachmentDescription attachment_descriptions[] = 
    {
        /* Color attachment */
        {
            .flags          = 0,
            .format         = VK_FORMAT_R8G8B8_SNORM,
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
            .format         = VK_FORMAT_R8G8B8A8_SNORM,
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

void Example::createFrameBuffers(void)
{
    VkResult result;
    VkFramebufferCreateInfo fci =
    {
        .sType              = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = 0,
        .renderPass         = m_renderPass,
        .attachmentCount    = 0u,
        .pAttachments       = nullptr,
        .width              = m_surfaceCapabilities.currentExtent.width,
        .height             = m_surfaceCapabilities.currentExtent.height,
        .layers             = 1u,
    };

    result = vkCreateFramebuffer(m_device, &fci, nullptr, &m_framebuffer);
    printResult(result, "Framebuffer creation result");
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

#if 1
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
    VkResult result;

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
        {.x = 0.0f, .y = 0.0f, .width = 640.0f, .height = 480.0f, .minDepth = 0.5f, .maxDepth = 1000.0f}
    };

    VkPipelineViewportStateCreateInfo pvsci = 
    {
        .sType          = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext          = nullptr,
        .flags          = 0,
        .viewportCount  = 1u,
        .pViewports     = vp,
        .scissorCount   = 0u,
        .pScissors      = nullptr
    };

    VkPipelineRasterizationStateCreateInfo prsci = 
    {
        .sType                      = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                      = nullptr,
        .flags                      = 0,
        .depthClampEnable           = VK_TRUE,
        .rasterizerDiscardEnable    = VK_TRUE,
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
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
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
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &pcbas,
        .blendConstants = {0.f, 0.f, 0.f, 0.f},
    };

    VkPipelineDynamicStateCreateInfo pdsci = 
    {

    };

    VkGraphicsPipelineCreateInfo ci[] =
    {
        {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT,
        .stageCount             = sizeof(shaderStagesInfo) / sizeof(shaderStagesInfo[0]),
        .pStages                = shaderStagesInfo,
        .pVertexInputState      = &pvisci,
        .pInputAssemblyState    = &piasci,
        .pTessellationState     = nullptr,
        .pViewportState         = &pvsci,
        .pRasterizationState    = &prsci,
        .pMultisampleState      = nullptr,//&pmssci,
        .pDepthStencilState     = nullptr,//&pdssci,
        .pColorBlendState       = nullptr,//&pcbsci,
        .pDynamicState          = nullptr,
        .layout                 = m_pipelineLayout,
        .renderPass             = m_renderPass,
        .subpass                = 0u,
        .basePipelineHandle     = VK_NULL_HANDLE,
        .basePipelineIndex      = 0u,
        }
    };

    m_pipelines.resize(1u);
    result = vkCreateGraphicsPipelines(m_device,
                                       VK_NULL_HANDLE,
                                       1,
                                       ci,
                                       nullptr,
                                       &m_pipelines[0]);
    printResult(result, "Graphics pipeline creation result");
}
#else
void Example::createPipeline(void)
{

}
#endif

void Example::createCommandBuffers(void)
{
#if 0
    VkResult result;

    VkCommandPoolCreateInfo cpci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = getQueueFamilyIndex(),
    };

    vkCreateCommandPool(m_device, &cpci, NULL, &m_commandPool);

    m_commandBuffers.resize(1u);

    VkCommandBufferAllocateInfo cbai = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = NULL,
        .commandPool        = m_commandPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = m_commandBuffers.size(),
    };
    vkAllocateCommandBuffers(m_device, &cbai, m_commandBuffers.data());

    VkCommandBufferBeginInfo cbbi = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    VkRenderPassBeginInfo rpbi = {
        .sType          = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext          = NULL,
        .renderPass     = m_renderPass,
        .framebuffer    = ,
        .renderArea     = ,
        .clearValueCount = ,
        .pClearValues   = ,
    };

    for (uint8_t i = 0u; i < m_commandBuffers.size(); i++)
    {
        vkBeginCommandBuffer(m_commandBuffers[i], &cbbi);

        vkCmdBeginRenderPass(m_commandBuffers[i], &rpbi, VK_SUBPASS_CONTENTS_INLINE);

        result = vkEndCommandBuffer(m_commandBuffers[i]);
    }
#endif
}

void Example::DrawFrame(void)
{
    VkResult result;
    uint32_t imageIndex;
    VkPresentInfoKHR presentInfo;

    result = vkAcquireNextImageKHR(m_device, m_swapchain, 0u, drawSemaphore, drawFence, &imageIndex);

    /* Use sync primitive so we don't modify image being read from */
    if (VK_SUCCESS == result)
    {
        /* Queue all rendering commands and transition the image layout  */
        VkSemaphore waitSemaphores[] = {};
        VkSemaphore signalSemaphores[] = {};
        VkPipelineStageFlags pipelineStageFlags[] = {};
        VkFence fence;

        VkSubmitInfo submitInfo = {
            .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext              = NULL,
            .waitSemaphoreCount = 1u,
            .pWaitSemaphores    = waitSemaphores,
            .pWaitDstStageMask  = pipelineStageFlags,
            .commandBufferCount = (uint32_t)m_commandBuffers.size(),
            .pCommandBuffers    = m_commandBuffers.data(),
            .signalSemaphoreCount = 1u,
            .pSignalSemaphores  = signalSemaphores,
        };
        vkQueueSubmit(queue, 1u, &submitInfo, fence);

        /* Queue the image for presentation */
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = NULL;
        presentInfo.waitSemaphoreCount = 1u;
        presentInfo.pWaitSemaphores = &drawSemaphore;
        presentInfo.swapchainCount = 1u;
        presentInfo.pSwapchains = &m_swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = NULL;

        result = vkQueuePresentKHR(queue, &presentInfo);
    }
}

void Example::cleanup(void)
{
    /* Command pool may be freed only after all command buffers are in pending???/init state??? */
    //vkDestroyCommandPool(m_device, m_commandPool, NULL);
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    for (auto imageView : m_swapchainImageViews)
    {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();
    vkDestroyInstance(m_instance, nullptr);
}