#pragma once

#include <glm/glm.hpp>

class DirectionLight
{
 public:
	glm::vec3 Direction;
  glm::vec3 Color;

  DirectionLight(glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3 direction = glm::vec3(0.0f))
	{
		Direction = direction;
		Color = color;
	}
};
