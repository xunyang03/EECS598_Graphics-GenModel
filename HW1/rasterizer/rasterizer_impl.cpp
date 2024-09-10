#include <cstdint>

#include "image.hpp"
#include "loader.hpp"
#include "rasterizer.hpp"

// TODO
void Rasterizer::DrawPixel(uint32_t x, uint32_t y, Triangle trig, AntiAliasConfig config, uint32_t spp, Image& image, Color color)
{
    if (config == AntiAliasConfig::NONE)            // if anti-aliasing is off
    {
        if (threeXProduct(x, y, trig)
            image.Set(x, y, color);                 // write to the corresponding pixel
    }
    else if (config == AntiAliasConfig::SSAA)       // if anti-aliasing is on
    {

    }

    // if the pixel is inside the triangle
    image.Set(x, y, color);

    return;
}

bool threeXProduct(uint32_t x, uint32_t y, Triangle trig) {
    glm::vec2 A1(trig.pos[0].x, trig.pos[0].y);
    glm::vec2 A2(trig.pos[1].x, trig.pos[1].y);
    glm::vec2 A3(trig.pos[2].x, trig.pos[2].y);
    glm::vec2 X(x, y);

    float cross1 = cross2D((X - A1), (A2 - A1));
    float cross2 = cross2D((X - A2), (A3 - A2));
    float cross3 = cross2D((X - A3), (A1 - A3));
    // inside the triangle if and only if the three cross products have the same sign
    return (cross1 >= 0 && cross2 >= 0 && cross3 >= 0) || (cross1 < 0 && cross2 < 0 && cross3 < 0); 
}

float cross2D(const glm::vec2& v1, const glm::vec2& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

// TODO
void Rasterizer::AddModel(MeshTransform transform, glm::mat4 rotation)
{
    /* model.push_back( model transformation constructed from translation, rotation and scale );*/
    return;
}

// TODO
void Rasterizer::SetView()
{
    const Camera& camera = this->loader.GetCamera();
    glm::vec3 cameraPos = camera.pos;
    glm::vec3 cameraLookAt = camera.lookAt;

    // TODO change this line to the correct view matrix
    this->view = glm::mat4(1.);

    return;
}

// TODO
void Rasterizer::SetProjection()
{
    const Camera& camera = this->loader.GetCamera();

    float nearClip = camera.nearClip;                   // near clipping distance, strictly positive
    float farClip = camera.farClip;                     // far clipping distance, strictly positive
    
    float width = this->loader.GetWidth();
    float height = this->loader.GetHeight();
    
    // TODO change this line to the correct projection matrix
    this->projection = glm::mat4(1.);

    return;
}

// TODO
void Rasterizer::SetScreenSpace()
{
    float width = this->loader.GetWidth();
    float height = this->loader.GetHeight();

    // TODO change this line to the correct screenspace matrix
    this->screenspace = glm::mat4(1.);

    return;
}

// TODO
glm::vec3 Rasterizer::BarycentricCoordinate(glm::vec2 pos, Triangle trig)
{
    return glm::vec3();
}

// TODO
float Rasterizer::zBufferDefault = float();

// TODO
void Rasterizer::UpdateDepthAtPixel(uint32_t x, uint32_t y, Triangle original, Triangle transformed, ImageGrey& ZBuffer)
{

    float result;
    ZBuffer.Set(x, y, result);

    return;
}

// TODO
void Rasterizer::ShadeAtPixel(uint32_t x, uint32_t y, Triangle original, Triangle transformed, Image& image)
{

    Color result;
    image.Set(x, y, result);

    return;
}
