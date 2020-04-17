#pragma once

#include "../../../shader.h"
#include "direction_light.h"
#include "point_light.h"

class LightUtils
{
 public:
	static void SetupDirectionLight(DirectionLight * directionLight, Shader * shader, const std::string &name)
	{
		shader->setVec3(name + ".direction", directionLight->Direction);
		shader->setVec3(name + ".ambient", directionLight->Color * 0.05f);
		shader->setVec3(name + ".diffuse", directionLight->Color * 0.4f);
		shader->setVec3(name + ".specular", directionLight->Color * 0.5f);
	}
	
  static void SetupPointLight(PointLight *pointLight, Shader* shader, const std::string &name)
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
