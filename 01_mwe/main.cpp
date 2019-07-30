#include "example.hpp"
#include <cmath>

#include "glm/glm/vec3.hpp"
#include "glm/glm/vec4.hpp"
#include "glm/glm/mat4x4.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"


extern Vertex my_cube[36];

glm::mat4 camera(float Translate, glm::vec2 const &Rotate)
{
    #if 0
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(320.0f, 240.0f, -50.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    model = glm::rotate(model, (float)(m_pi/4.), glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::rotate(model, (float)(m_pi/2.), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-320.0f, -240.0f, -50.f));

    glm::mat4 projection = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 1000.0f);

    //glm::mat4 projection = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, 0.1f, 100.0f);
    return projection * view * model; 
    #else
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 1.0f, -50.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    model = glm::rotate(model, (float)(glm::radians(45.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, (float)(glm::radians(15.0f)), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-0.5f, -0.5f, 100.f));

    glm::mat4 projection = glm::scale(glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 1000.0f), glm::vec3(1.0f, 1.0f, -1.0f));

    //glm::mat4 projection = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, 0.1f, 100.0f);
    return projection * view * model; 
    #endif
}

int main(void)
{
    Example vulkan_example;
    /* compute new coordinates */
    uint32_t numberOfVertices = Example::getCubeVerticesCount();

    glm::mat4 my_camera = camera(0.f, glm::vec2(0.f, 0.f));

    Vertex tmp_cube[numberOfVertices];
    memcpy(tmp_cube, my_cube, Example::getCubeSizeBytes());

    for (uint32_t i = 0; i < numberOfVertices; i++)
    {
        #if 1
        my_cube[i].coord = my_camera * tmp_cube[i].coord;
        #endif
    }

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