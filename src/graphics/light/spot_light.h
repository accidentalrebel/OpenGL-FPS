#pragma once

#include <glm/glm.hpp>
#include "../shader.h"

class SpotLight
{
 public:
  glm::vec3 Position;
	glm::vec3 Direction;
  glm::vec3 Color;

	float AmbientIntensity = 1.0f;
	float DiffuseIntensity = 1.0f;
	float SpecularIntensity = 1.0f;
	
	float CutOff = 12.5f;
	float OuterCutOff = 15.0f;

	float Constant = 1.0f;
	float Linear = 0.35f;
	float Quadratic = 0.44f;

  SpotLight(glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f), float cutOff = 0.0f, float outerCutOff = 0.0f)
	{
		Color = color;
		CutOff = cutOff;
		OuterCutOff = outerCutOff;
  }

	void setup(Shader* shader, const std::string &uniformName)
	{
		shader->setVec3(uniformName + ".position", Position);
		shader->setVec3(uniformName + ".direction", Direction);
		shader->setFloat(uniformName + ".cutOff", glm::cos(glm::radians(CutOff)));
		shader->setFloat(uniformName + ".outerCutOff", glm::cos(glm::radians(OuterCutOff)));
		shader->setVec3(uniformName + ".ambient", Color * AmbientIntensity);
		shader->setVec3(uniformName + ".diffuse", Color * DiffuseIntensity);
		shader->setVec3(uniformName + ".specular", Color * SpecularIntensity);
		shader->setFloat(uniformName + ".constant", Constant);
    shader->setFloat(uniformName + ".linear", Linear);
    shader->setFloat(uniformName + ".quadratic", Quadratic);
		shader->setBool("isSpotLightSetup", true);
	}
};
