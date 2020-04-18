#pragma once

#include "../shader.h"
#include "direction_light.h"
#include "point_light.h"
#include "spot_light.h"

class LightUtils
{
 public:
	static void SetupDirectionLight(DirectionLight * directionLight, Shader * shader, const std::string &name)
	{
		shader->setVec3(name + ".direction", directionLight->Direction);
		shader->setVec3(name + ".ambient", directionLight->Color * directionLight->AmbientIntensity);
		shader->setVec3(name + ".diffuse", directionLight->Color * directionLight->DiffuseIntensity);
		shader->setVec3(name + ".specular", directionLight->Color * directionLight->SpecularIntensity);
	}
	
  static void SetupPointLight(PointLight *pointLight, Shader* shader, const std::string &name)
  {
    shader->setVec3(name + ".position", pointLight->Position);
    shader->setVec3(name + ".ambient", pointLight->Color * pointLight->AmbientIntensity);
    shader->setVec3(name + ".diffuse", pointLight->Color * pointLight->DiffuseIntensity);
    shader->setVec3(name + ".specular", pointLight->Color);
    shader->setFloat(name + ".constant", pointLight->Constant);
    shader->setFloat(name + ".linear", pointLight->Linear);
    shader->setFloat(name + ".quadratic", pointLight->Quadratic);
  }

	static void SetupSpotLight(SpotLight *spotLight, Shader* shader, const std::string &name)
	{
		shader->setVec3(name + "position", spotLight->Position);
		shader->setVec3(name + "direction", spotLight->Direction);
		shader->setFloat(name + "cutOff", glm::cos(glm::radians(spotLight->CutOff)));
		shader->setFloat(name + "outerCutOff", glm::cos(glm::radians(spotLight->OuterCutOff)));
		shader->setVec3(name + "ambient", spotLight->Color * spotLight->AmbientIntensity);
		shader->setVec3(name + "diffuse", spotLight->Color * spotLight->DiffuseIntensity);
		shader->setVec3(name + "specular", spotLight->Color);
	}
};
