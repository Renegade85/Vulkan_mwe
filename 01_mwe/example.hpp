#include <string>
#include <vector>

#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"

#ifndef EXAMPLE_GUARD
#define EXAMPLE_GUARD

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

#define COLOR_RED   {1.f, 0.f, 0.f, 1.f}
#define COLOR_GREEN {0.f, 1.f, 0.f, 1.f}
#define COLOR_BLUE  {0.f, 0.f, 1.f, 1.f}
#define COLOR_RG    {1.f, 1.f, 0.f, 1.f}
#define COLOR_RB    {1.f, 0.f, 1.f, 1.f}
#define COLOR_GB    {0.f, 1.f, 1.f, 1.f}

#define VERTEX_1    {0.f, 0.f, 0.f, 1.f}
#define VERTEX_2    {1.f, 0.f, 0.f, 1.f}
#define VERTEX_3    {1.f, 1.f, 0.f, 1.f}
#define VERTEX_4    {0.f, 1.f, 0.f, 1.f}
#define VERTEX_5    {1.f, 0.f, 1.f, 1.f}
#define VERTEX_6    {0.f, 0.f, 1.f, 1.f}
#define VERTEX_7    {1.f, 1.f, 1.f, 1.f}
#define VERTEX_8    {0.f, 1.f, 1.f, 1.f}

typedef enum
{
    DOUBLE_BUFFERING,
    TRIPPLE_BUFFERING
} eBufferingMode;

class Example
{
    private:
        GLFWwindow *    m_window;
        VkSurfaceKHR    m_surface;
        VkSurfaceCapabilitiesKHR        m_surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
        std::vector<VkPresentModeKHR>   m_presentModes;
    
        VkInstance      m_instance;
        std::vector<VkExtensionProperties> m_available_extensions;
        std::vector<const char *> m_required_validation_layers = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_standard_validation", "VK_LAYER_LUNARG_core_validation"};

        std::vector<const char *> m_required_instance_extensions = {VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface"};

        std::vector<VkPhysicalDevice>       m_available_devices;
        std::vector<const char *>  m_requiredPhysicalDeviceExtension = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkDevice        m_device;
        uint32_t        m_selected_device;
        uint32_t        m_graphics_queue_idx;
        uint32_t        m_present_queue_idx;

        VkFramebuffer m_framebuffer;

        VkSwapchainKHR              m_swapchain;
        VkBool32                    m_isDoubleBufferingSupported;
        VkBool32                    m_isTrippleBufferingSupported;
        eBufferingMode              m_selectedBufferingMode;
        std::vector<VkImage>        m_swapchainImages;
        std::vector<VkImageView>    m_swapchainImageViews;

        VkSemaphore drawSemaphore;
        VkFence drawFence;
        VkQueue queue;

        std::vector<VkPipeline> m_pipelines;
        VkPipelineLayout m_pipelineLayout;

        VkRenderPass m_renderPass;
        VkAttachmentDescription m_attachmentDescription;
        VkSubpassDescription m_subpassDescriptions;

        std::string m_vertexShaderCode = "";
        std::string m_fragmentShaderCode = "";

        VkCommandPool m_commandPool;
        std::vector<VkCommandBuffer> m_commandBuffers;
        
    public:
        void DrawFrame(void);
        void InitExample(void);

        int32_t createWindow(void);

        void run(void);
        
        void createInstance(void);
        void createDevice(void);
        void createSwapchain(void);
        void createImageViews(void);
        void createRenderPass(void);
        void createFrameBuffers(void);
        void createCommandBuffers(void);
        void createPipeline(void);

        uint32_t getQueueFamilyIndex(void);

        void cleanup(void);
};
#endif