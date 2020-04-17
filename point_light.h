#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <glm/glm.hpp>

class PointLight
{
 public:
  glm::vec3 Position;
  glm::vec3 Color;

  PointLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f))
	{
		Position = position;
		Color = color;
  }
};
#endif // POINTLIGHT_H
