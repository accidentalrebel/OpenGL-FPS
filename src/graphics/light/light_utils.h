#pragma once

#include "../../../shader.h"
#include "direction_light.h"
#include "point_light.h"

class LightUtils
{
 public:
	static void SetupDirectionLight(DirectionLight * directionLight, Shader * shader)
	{
		shader->setVec3("dirLight.direction", directionLight->Direction);
		shader->setVec3("dirLight.ambient", directionLight->Color * 0.05f);
		shader->setVec3("dirLight.diffuse", directionLight->Color * 0.4f);
		shader->setVec3("dirLight.specular", directionLight->Color * 0.5f);
	}
	
  static void SetupPointLight(PointLight *pointLight, Shader* shader, const std::string &name, float ambientIntensity, float diffuseIntensity)
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
