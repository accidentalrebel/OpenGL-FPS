#pragma once

#include <glm/glm.hpp>
#include "../shader.h"

class PointLight
{
 public:
  glm::vec3 Position;
  glm::vec3 Color;

	float AmbientIntensity = 0.1f;
	float DiffuseIntensity = 0.8f;
	float SpecularIntensity = 0.5f;

	float Constant = 1.0f;
	float Linear = 0.35f;
	float Quadratic = 0.44f;

  PointLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f))
	{
		Position = position;
		Color = color;
  }

	void setup(Shader *shader, const std::string &uniformName)
	{
    shader->setVec3(uniformName + ".position", Position);
    shader->setVec3(uniformName + ".ambient", Color * AmbientIntensity);
    shader->setVec3(uniformName + ".diffuse", Color * DiffuseIntensity);
    shader->setVec3(uniformName + ".specular", Color * SpecularIntensity);
    shader->setFloat(uniformName + ".constant", Constant);
    shader->setFloat(uniformName + ".linear", Linear);
    shader->setFloat(uniformName + ".quadratic", Quadratic);
	}
};
