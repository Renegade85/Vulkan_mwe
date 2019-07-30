#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"
#include "glm/glm/vec4.hpp"

#ifndef EXAMPLE_GUARD
#define EXAMPLE_GUARD

#define USE_GLM

#if defined USE_GLM
struct Vertex
{
    glm::vec4 coord;
    glm::vec4 color;
};
#else
struct Vertex
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    } coord;
    struct
    {
        float r;
        float g;
        float b;
        float a;
    } color;
};
#endif

#define COLOR_RED   {1.f, 0.f, 0.f, 1.f}
#define COLOR_GREEN {0.f, 1.f, 0.f, 1.f}
#define COLOR_BLUE  {0.f, 0.f, 1.f, 1.f}
#define COLOR_RG    {1.f, 1.f, 0.f, 1.f}
#define COLOR_RB    {1.f, 0.f, 1.f, 1.f}
#define COLOR_GB    {0.f, 1.f, 1.f, 1.f}

#if 0
#define VERTEX_1    {0.f, 0.f, 0.f, 1.f}
#define VERTEX_2    {1.f, 0.f, 0.f, 1.f}
#define VERTEX_3    {1.f, -1.f, 0.f, 1.f}
#define VERTEX_4    {0.f, -1.f, 0.f, 1.f}
#define VERTEX_5    {0.f, 0.f, 1.f, 1.f}
#define VERTEX_6    {1.f, 0.f, 1.f, 1.f}
#define VERTEX_7    {1.f, -1.f, 1.f, 1.f}
#define VERTEX_8    {0.f, -1.f, 1.f, 1.f}
#else
#define VERTEX_1    {-0.5f, 0.5f, 0.f, 1.f}
#define VERTEX_2    {0.5f,  0.5f, 0.f, 1.f}
#define VERTEX_3    {0.5f, -0.5f, 0.f, 1.f}
#define VERTEX_4    {-0.5f, -0.5f, 0.f, 1.f}
#define VERTEX_5    {-0.5f, 0.5f, 1.f, 1.f}
#define VERTEX_6    {0.5f, 0.5f, 1.f, 1.f}
#define VERTEX_7    {0.5f, -0.5f, 1.f, 1.f}
#define VERTEX_8    {-0.5f, -0.5f, 1.f, 1.f}
#endif


typedef enum
{
    DOUBLE_BUFFERING,
    TRIPPLE_BUFFERING
} eBufferingMode;

class Example
{
    private:
        GLFWwindow *                        m_window;
        VkSurfaceKHR                        m_surface;
        VkSurfaceCapabilitiesKHR            m_surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR>     m_surfaceFormats;
        std::vector<VkPresentModeKHR>       m_presentModes;
    
        VkInstance                          m_instance;
        std::vector<VkExtensionProperties>  m_available_extensions;
        std::vector<const char *>           m_required_validation_layers = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_standard_validation"};

        std::vector<const char *>           m_required_instance_extensions = {VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface"};

        std::vector<VkPhysicalDevice>       m_available_devices;
        std::vector<const char *>           m_requiredPhysicalDeviceExtension = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkDevice        m_device;
        uint32_t        m_selected_device;
        uint32_t        m_graphics_queue_idx;
        uint32_t        m_present_queue_idx;

        VkSwapchainKHR              m_swapchain;
        VkBool32                    m_isDoubleBufferingSupported;
        VkBool32                    m_isTrippleBufferingSupported;
        eBufferingMode              m_selectedBufferingMode;
        std::vector<VkImage>        m_swapchainImages;
        std::vector<VkImageView>    m_swapchainImageViews;
        VkImage                     m_depthImage;
        VkDeviceMemory              m_depthImageMemory;
        VkImageView                 m_depthImageView;

        std::vector<VkFramebuffer>  m_framebuffers;

        std::vector<VkSemaphore>    m_imageReadySemaphores;
        VkSemaphore                 m_renderDoneSemaphore;
        std::vector<VkFence>        m_drawFences;
        uint32_t                    m_submissionNumber = 0u;
        uint32_t                    m_maxInflightSubmissions = 2u;

        VkBuffer        m_modelBuffer;
        VkDeviceMemory  m_modelBufferMemory;

        std::vector<VkPipeline> m_pipelines;
        VkPipelineLayout m_pipelineLayout;

        VkRenderPass m_renderPass;
        VkAttachmentDescription m_attachmentDescription;
        VkSubpassDescription m_subpassDescriptions;

        VkCommandPool m_commandPool;
        std::vector<VkCommandBuffer> m_commandBuffers;
        
    public:
        void drawFrame(void);
        void InitExample(void);

        int32_t createWindow(void);

        void run(void);
        
        void createInstance(void);
        void createDevice(void);
        void createSwapchain(void);
        void createDepthResources(void);
        void createImageViews(void);
        void createRenderPass(void);
        void createFramebuffers(void);
        void createCommandBuffers(void);
        void createPipeline(void);
        void createSemaphores(void);
        void createFences(void);

        uint32_t getQueueFamilyIndex(void);

        void cleanup(void);

        static uint32_t getCubeSizeBytes(void);
        static uint32_t getCubeVerticesCount(void);
};
#endif