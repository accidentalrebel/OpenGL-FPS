#pragma once

#include <glm/glm.hpp>
#include "../shader.h"

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

	void setup(Shader *shader, const std::string &uniformName)
	{
		shader->setVec3(uniformName + ".direction", Direction);
		shader->setVec3(uniformName + ".ambient", Color * AmbientIntensity);
		shader->setVec3(uniformName + ".diffuse", Color * DiffuseIntensity);
		shader->setVec3(uniformName + ".specular", Color * SpecularIntensity);
	}
};
