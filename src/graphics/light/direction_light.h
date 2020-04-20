#pragma once

#include <glm/glm.hpp>

class DirectionLight
{
 public:
	glm::vec3 Direction;
  glm::vec3 Color;

	float AmbientIntensity = 0.05f;
	float DiffuseIntensity = 0.4f;
	float SpecularIntensity = 0.5f;

  DirectionLight(glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3 direction = glm::vec3(0.0f))
	{
		Direction = direction;
		Color = color;
	}
};