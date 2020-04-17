#pragma once

#include "../../point_light.h"

class LightUtils
{
 public:
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
