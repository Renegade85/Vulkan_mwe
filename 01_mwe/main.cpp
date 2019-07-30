#include "example.hpp"
#include <cmath>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Matrix
{
public:
    float m[16u] = {0.f};
};

class MatrixTranslation : public Matrix
{
    public:
    MatrixTranslation(double dx, double dy, double dz)
    {
        m[0] = 1.f;
        m[5] = 1.f;
        m[10] = 1.f;
        m[12] = dx;
        m[13] = dy;
        m[14] = dz;
        m[15] = 1.f;
    }

    MatrixTranslation(float v4[4])
    {
        m[0] = 1.f;
        m[5] = 1.f;
        m[10] = 1.f;
        m[12] = v4[0];
        m[13] = v4[1];
        m[14] = v4[2];
        m[15] = 1.f;
    }
};

class MatrixRotation : public Matrix
{
public:
    MatrixRotation(float angle_deg, uint8_t axis)
    {
        switch (axis)
        {
        case 0: // x axis
            m[0] = 1.f;
            m[15] = 1.f;
            m[5] = cos(angle_deg * M_PI / 180.);   // cos angle
            m[6] = -sin(angle_deg * M_PI / 180.);  // -sin angle
            m[9] = sin(angle_deg * M_PI / 180.);   // sin angle
            m[10] = cos(angle_deg * M_PI / 180.); // cos angle
            break;
        case 1: // y axis
            m[5] = 1.f;
            m[15] = 1.f;
            m[0] = cos(angle_deg * M_PI / 180.);   // cos angle
            m[2] = sin(angle_deg * M_PI / 180.);   // sin angle
            m[8] = -sin(angle_deg * M_PI / 180.);  // -sin angle
            m[10] = cos(angle_deg * M_PI / 180.);  // cos angle
            break;
        case 3: // z axis
        default:
            break;
        }
    }
};

Matrix multiplyMatrices(Matrix A, Matrix B)
{
    Matrix result;

    /* 1st row */
    result.m[0] = A.m[0] * B.m[0] + A.m[1] * B.m[4] + A.m[2] * B.m[8] + A.m[3] * B.m[12];
    result.m[1] = A.m[0] * B.m[1] + A.m[1] * B.m[5] + A.m[2] * B.m[9] + A.m[3] * B.m[13];
    result.m[2] = A.m[0] * B.m[2] + A.m[1] * B.m[6] + A.m[2] * B.m[10] + A.m[3] * B.m[14];
    result.m[3] = A.m[0] * B.m[3] + A.m[1] * B.m[7] + A.m[2] * B.m[11] + A.m[3] * B.m[15];

    /* 2nd row */
    result.m[4] = A.m[4] * B.m[0] + A.m[5] * B.m[4] + A.m[6] * B.m[8] + A.m[7] * B.m[12];
    result.m[5] = A.m[4] * B.m[1] + A.m[5] * B.m[5] + A.m[6] * B.m[9] + A.m[7] * B.m[13];
    result.m[6] = A.m[4] * B.m[2] + A.m[5] * B.m[6] + A.m[6] * B.m[10] + A.m[7] * B.m[14];
    result.m[7] = A.m[4] * B.m[3] + A.m[5] * B.m[7] + A.m[6] * B.m[11] + A.m[7] * B.m[15];

    /* 3rd row */
    result.m[8] = A.m[8] * B.m[0] + A.m[9] * B.m[4] + A.m[10] * B.m[8] + A.m[11] * B.m[12];
    result.m[9] = A.m[8] * B.m[1] + A.m[9] * B.m[5] + A.m[10] * B.m[9] + A.m[11] * B.m[13];
    result.m[10] = A.m[8] * B.m[2] + A.m[9] * B.m[6] + A.m[10] * B.m[10] + A.m[11] * B.m[14];
    result.m[11] = A.m[8] * B.m[3] + A.m[9] * B.m[7] + A.m[10] * B.m[11] + A.m[11] * B.m[15];

    /* 4th row */
    result.m[12] = A.m[12] * B.m[0] + A.m[13] * B.m[4] + A.m[14] * B.m[8] + A.m[15] * B.m[12];
    result.m[13] = A.m[12] * B.m[1] + A.m[13] * B.m[5] + A.m[14] * B.m[9] + A.m[15] * B.m[13];
    result.m[14] = A.m[12] * B.m[2] + A.m[13] * B.m[6] + A.m[14] * B.m[10] + A.m[15] * B.m[14];
    result.m[15] = A.m[12] * B.m[3] + A.m[13] * B.m[7] + A.m[14] * B.m[11] + A.m[15] * B.m[15];

    return result;
}

void multiplyMatrixVertex(Matrix &A, Vertex &v, Vertex *result)
{
    result->coord.x = A.m[0] * v.coord.x + A.m[1] * v.coord.y + A.m[2] * v.coord.z + A.m[3] * v.coord.w;   
    result->coord.y = A.m[4] * v.coord.x + A.m[5] * v.coord.y + A.m[6] * v.coord.z + A.m[7] * v.coord.w;   
    result->coord.z = A.m[8] * v.coord.x + A.m[9] * v.coord.y + A.m[10] * v.coord.z + A.m[11] * v.coord.w;   
    result->coord.w = A.m[12] * v.coord.x + A.m[13] * v.coord.y + A.m[14] * v.coord.z + A.m[15] * v.coord.w;   
}

extern Vertex my_cube[];

glm::mat4 camera(float Translate, glm::vec2 const &Rotate)
{
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
    View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    return Projection * View * Model;
}

int main(void)
{
    Example vulkan_example;
    /* compute new coordinates */
    uint32_t numberOfVertices = Example::getCubeVerticesCount();

    glm::mat4 my_camera = camear(0.f, glm::vec2(0.f, 0.f));

    Matrix m_rot_x = MatrixRotation(0, 0);
    Matrix m_rot_y = MatrixRotation(45, 1);
    Matrix m_transform;

    Vertex tmp_cube[numberOfVertices];
    memcpy(tmp_cube, my_cube, Example::getCubeSizeBytes());

    for (uint32_t i = 0; i < numberOfVertices; i++)
    {
        #if 0
        Matrix m_t1 = MatrixTranslation(-my_cube[0].coord.x, -my_cube[0].coord.y, -my_cube[0].coord.z);
        Matrix m_t2 = MatrixTranslation(my_cube[0].coord.x, my_cube[0].coord.y, my_cube[0].coord.z);

        m_transform = multiplyMatrices(m_t1, m_rot_x);
        m_transform = multiplyMatrices(m_transform, m_rot_y);
        m_transform = multiplyMatrices(m_transform, m_t2);

        multiplyMatrixVertex(m_transform, tmp_cube[i], &my_cube[i]);
        #else
        Matrix m_t1 = MatrixTranslation(-my_cube[0].coord.x, -my_cube[0].coord.y, -my_cube[0].coord.z);
        Matrix m_t2 = MatrixTranslation(my_cube[0].coord.x, my_cube[0].coord.y, my_cube[0].coord.z);

        m_transform = multiplyMatrices(m_t1, m_rot_y);
        m_transform = multiplyMatrices(m_transform, m_t2);

        

        multiplyMatrixVertex(m_transform, tmp_cube[i], &my_cube[i]);
        //multiplyMatrixVertex(m_transform, my_cube[i], &tmp_cube[i]);
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