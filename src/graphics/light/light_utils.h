#pragma once

#include "../shader.h"
#include "direction_light.h"
#include "point_light.h"
#include "spot_light.h"

class LightUtils
{
 public:
	static void SetupDirectionLight(DirectionLight * directionLight, Shader * shader, const std::string &uniformName)
	{
		shader->setVec3(uniformName + ".direction", directionLight->Direction);
		shader->setVec3(uniformName + ".ambient", directionLight->Color * directionLight->AmbientIntensity);
		shader->setVec3(uniformName + ".diffuse", directionLight->Color * directionLight->DiffuseIntensity);
		shader->setVec3(uniformName + ".specular", directionLight->Color * directionLight->SpecularIntensity);
	}
	
  static void SetupPointLight(PointLight *pointLight, Shader* shader, const std::string &uniformName)
  {
    shader->setVec3(uniformName + ".position", pointLight->Position);
    shader->setVec3(uniformName + ".ambient", pointLight->Color * pointLight->AmbientIntensity);
    shader->setVec3(uniformName + ".diffuse", pointLight->Color * pointLight->DiffuseIntensity);
    shader->setVec3(uniformName + ".specular", pointLight->Color * pointLight->SpecularIntensity);
    shader->setFloat(uniformName + ".constant", pointLight->Constant);
    shader->setFloat(uniformName + ".linear", pointLight->Linear);
    shader->setFloat(uniformName + ".quadratic", pointLight->Quadratic);
  }

	static void SetupSpotLight(SpotLight *spotLight, Shader* shader, const std::string &uniformName)
	{
		shader->setVec3(uniformName + ".position", spotLight->Position);
		shader->setVec3(uniformName + ".direction", spotLight->Direction);
		shader->setFloat(uniformName + ".cutOff", glm::cos(glm::radians(spotLight->CutOff)));
		shader->setFloat(uniformName + ".outerCutOff", glm::cos(glm::radians(spotLight->OuterCutOff)));
		shader->setVec3(uniformName + ".ambient", spotLight->Color * spotLight->AmbientIntensity);
		shader->setVec3(uniformName + ".diffuse", spotLight->Color * spotLight->DiffuseIntensity);
		shader->setVec3(uniformName + ".specular", spotLight->Color * spotLight->SpecularIntensity);
		shader->setFloat(uniformName + ".constant", spotLight->Constant);
    shader->setFloat(uniformName + ".linear", spotLight->Linear);
    shader->setFloat(uniformName + ".quadratic", spotLight->Quadratic);
		shader->setBool("isSpotLightSetup", true);
	}
};
