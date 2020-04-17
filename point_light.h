#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <glm/glm.hpp>
#include "shader.h"

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

	static void Setup(PointLight *pointLight, Shader* shader, const std::string &name, float ambientIntensity, float diffuseIntensity)
	{
		shader->setVec3(name + ".position", pointLight->Position);
		shader->setVec3(name + ".ambient", pointLight->Color * 0.1f);
		shader->setVec3(name + ".diffuse", pointLight->Color * 0.8f);
		shader->setVec3(name + ".specular", pointLight->Color);
		shader->setFloat(name + ".constant", 1.0f);
		shader->setFloat(name + ".linear", 0.09);
		shader->setFloat(name + ".quadratic", 0.032);
	}
};
#endif // POINTLIGHT_H
