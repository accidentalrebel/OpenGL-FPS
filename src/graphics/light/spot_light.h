#pragma once

#include <glm/glm.hpp>

class SpotLight
{
 public:
  glm::vec3 Position;
	glm::vec3 Direction;
  glm::vec3 Color;

	float AmbientIntensity = 0.1f;
	float DiffuseIntensity = 0.8f;
	float CutOff;
	float OuterCutOff;

  SpotLight(glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f), float cutOff = 0.0f, float outerCutOff = 0.0f)
	{
		Color = color;
		CutOff = cutOff;
		OuterCutOff = outerCutOff;
  }
};
