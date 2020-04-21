#pragma once

#include <glm/glm.hpp>

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
};
