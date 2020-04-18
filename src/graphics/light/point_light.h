#pragma once

#include <glm/glm.hpp>

class PointLight
{
 public:
  glm::vec3 Position;
  glm::vec3 Color;

	float AmbientIntensity = 0.1f;
	float DiffuseIntensity = 0.8f;
	float Constant = 1.0f;
	float Linear = 0.35f;
	float Quadratic = 0.44f;

  PointLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f))
	{
		Position = position;
		Color = color;
  }
};
