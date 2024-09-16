#include <cstdint>
#include <vector>
#include <cmath>
#include <iostream>
#include "image.hpp"
#include "loader.hpp"
#include "rasterizer.hpp"

// 2D vector cross product
float cross2D(const glm::vec2& v1, const glm::vec2& v2) {
	return v1.x * v2.y - v1.y * v2.x;
}

// judge whether a point is inside the triangle
template<typename T> bool threeXProduct(T x, T y, Triangle trig) {
	glm::vec2 A1(trig.pos[0].x, trig.pos[0].y);
	glm::vec2 A2(trig.pos[1].x, trig.pos[1].y);
	glm::vec2 A3(trig.pos[2].x, trig.pos[2].y);
	glm::vec2 X(x, y);
	float cross1 = cross2D((X - A1), (A2 - A1));
	float cross2 = cross2D((X - A2), (A3 - A2));
	float cross3 = cross2D((X - A3), (A1 - A3));
	// X is inside the triangle if and only if the three cross products have the same sign
	return (cross1 >= 0 && cross2 >= 0 && cross3 >= 0) || (cross1 < 0 && cross2 < 0 && cross3 < 0);
}


// TODO
void Rasterizer::DrawPixel(uint32_t x, uint32_t y, Triangle trig, AntiAliasConfig config, uint32_t spp, Image& image, Color color)
{
	trig.Homogenize();
	if (config == AntiAliasConfig::NONE)            // if anti-aliasing is off
	{
		float x_c = x + 0.5f, y_c = y + 0.5f;
		if (threeXProduct(x_c, y_c, trig))
			image.Set(x, y, color);					// write to the corresponding pixel             
	}
	else if (config == AntiAliasConfig::SSAA)       // if anti-aliasing is on
	{
		int count = 0;
		for (auto i = 0; i < spp; i++) {
			for (auto j = 0; j < spp; j++) {
				float x_fine = x + (i + 0.5f) / spp;
				float y_fine = y + (j + 0.5f) / spp;
				if (threeXProduct(x_fine, y_fine, trig))
					count++;
			}
		}
		// update the color according to confidence
		Color color_fine = color * (static_cast<float>(count) / (spp * spp));
		image.Set(x, y, color_fine);
	}

	return;
}

// TODO
void Rasterizer::AddModel(MeshTransform transform, glm::mat4 rotation)
{
	glm::mat4 M_rot = rotation;
	glm::vec3 translation = transform.translation, scale = transform.scale;
	glm::mat4 M_trans = glm::transpose(glm::mat4(
		1.0f, 0.0f, 0.0f, translation.x,
		0.0f, 1.0f, 0.0f, translation.y,
		0.0f, 0.0f, 1.0f, translation.z,
		0.0f, 0.0f, 0.0f, 1.0f));
	glm::mat4 M_scale = glm::transpose(glm::mat4(
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y,  0.0f, 0.0f,
		0.0f, 0.0f, scale.z,  0.0f,
		0.0f, 0.0f, 0.0f, 1.0f));
	
	//model.push_back(M_trans * M_rot * M_scale); // T * R * S
	model.push_back(M_scale * M_trans * M_rot);
	return;
}

glm::vec3 normalizeVec(glm::vec3 vec) {
	float dist = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	if (dist == 0.0f)
		return glm::vec3(0.0f, 0.0f, 0.0f);  // Return zero vector in this case
	return glm::vec3(vec.x / dist, vec.y / dist, vec.z / dist);
}

// TODO
void Rasterizer::SetView()
{
    const Camera& camera = this->loader.GetCamera();
    glm::vec3 cameraPos = camera.pos;
    glm::vec3 cameraLookAt = camera.lookAt;
    glm::vec3 cameraUp = camera.up;

	glm::vec3 g = normalizeVec(cameraLookAt - cameraPos);
	glm::vec3 h = normalizeVec(cameraUp);
	glm::vec3 f = normalizeVec(glm::cross(g, h)); // binormal axis
	// check orthonormal
	glm::mat4 M_rot = glm::transpose(glm::mat4(
		f.x, f.y, f.z, 0.0f,
		h.x, h.y, h.z, 0.0f,
		-g.x, -g.y, -g.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f));
	glm::mat4 M_trans = glm::transpose(glm::mat4(
		1.0f, 0.0f, 0.0f, -cameraPos.x,
		0.0f, 1.0f, 0.0f, -cameraPos.y,
		0.0f, 0.0f, 1.0f, -cameraPos.z,
		0.0f, 0.0f, 0.0f, 1.0f));
	// TODO change this line to the correct view matrix
	this->view = M_rot * M_trans;

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

	// perspective projection
	glm::mat4 M_persp2ortho = glm::transpose(glm::mat4(
		nearClip, 0.0f, 0.0f, 0.0f,
		0.0f, nearClip, 0.0f, 0.0f,
		0.0f, 0.0f, nearClip + farClip, -nearClip * farClip,
		0.0f, 0.0f, 1.0f, 0.0f));
	glm::mat4 M_ortho_scale = glm::transpose(glm::mat4(
		2.0f / width, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / height, 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f / (nearClip - farClip), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f));
	glm::mat4 M_ortho_trans = glm::transpose(glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, -(nearClip + farClip)/2.0f,
		0.0f, 0.0f, 0.0f, 1.0f));
	glm::mat4 M_ortho2canon = M_ortho_scale * M_ortho_trans;
	this->projection = M_ortho2canon * M_persp2ortho;

	return;
}

// TODO
void Rasterizer::SetScreenSpace()
{
	float width = this->loader.GetWidth();
	float height = this->loader.GetHeight();

	glm::mat4 M_ss = glm::transpose(glm::mat4(
		width / 2.0f, 0.0f, 0.0f, (width )/2.0f,
		0.0f, height / 2.0f, 0.0f, (height )/2.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f));
	this->screenspace = M_ss;

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
