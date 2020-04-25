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

	void use(Shader *objectShader, const std::string &uniformName)
	{
		objectShader->use();
    objectShader->setVec3(uniformName + ".position", Position);
    objectShader->setVec3(uniformName + ".ambient", Color * AmbientIntensity);
    objectShader->setVec3(uniformName + ".diffuse", Color * DiffuseIntensity);
    objectShader->setVec3(uniformName + ".specular", Color * SpecularIntensity);
    objectShader->setFloat(uniformName + ".constant", Constant);
    objectShader->setFloat(uniformName + ".linear", Linear);
    objectShader->setFloat(uniformName + ".quadratic", Quadratic);
	}

	void setup(unsigned int vao)
	{
		this->VAO = vao;
	}

	void draw(Shader *lampShader)
	{
		lampShader->use();
		
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position);
		model = glm::scale(model, glm::vec3(0.2f));
		
		lampShader->setMat4("model", model);
		lampShader->setVec3("lightColor", Color);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

 private:
	unsigned int VAO;
};
