#include "example.hpp"

int main(void)
{
    Example vulkan_example;

    vulkan_example.createInstance();
    vulkan_example.createWindow();
    vulkan_example.createDevice();
    vulkan_example.createRenderPass();
    vulkan_example.createSwapchain();
    vulkan_example.createDepthResources();
    vulkan_example.createImageViews();
    vulkan_example.createRenderPass();
    vulkan_example.createPipeline();
    vulkan_example.createFramebuffers();
    vulkan_example.createCommandBuffers();
    vulkan_example.createSemaphores();
    vulkan_example.createFences();

    vulkan_example.run();

    vulkan_example.cleanup();

    return 0;
}