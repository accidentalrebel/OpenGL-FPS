#include <map>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include "src/graphics/camera.h"
#include "src/graphics/shader.h"
#include "src/graphics/model.h"
#include "src/graphics/light/direction_light.h"
#include "src/graphics/light/spot_light.h"
#include "src/graphics/light/point_light.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow *window);
void displayMap(Shader *shader, unsigned int VAO, unsigned int diffuseMap, unsigned int specularMap);
void getPositionFromTileIndex(unsigned int index, glm::vec3 *positions);
bool canMoveToPosition(glm::vec3 currentPosition);

glm::vec3 getTileCoords(glm::vec3 currentPosition, glm::vec3 centerOffset);
glm::vec3 castRay(glm::vec3, glm::vec3, float);
glm::vec3 handleObjectAtPos(glm::vec3);
unsigned int getTileAt(unsigned int col, unsigned int row);
void setTileAt(unsigned int col, unsigned int row, unsigned int value);
void setupLights(Shader *lampShader, Shader *objectShader, DirectionLight *directionLight, PointLight pointLights[], unsigned int pointLightCount, SpotLight *spotLight, unsigned int VAO);

void displayNanosuit(Model *nanosuit, Shader *shader);
void displayWindows(Shader *shader, unsigned int quadVAO, unsigned int texture);
void drawPlanet(Model *planet, Shader *shader, float scale);
void displayPlanet(Model *planet, Shader *, Shader *);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera g_camera(glm::vec3(2.0f, 0.0f, 4.0f));
float lastX = 400.0f, lastY = 300.0f;
float g_lastKeyPressed = 0;
bool g_firstMouse = true;

const unsigned int maxMarkerCount = 10;
glm::vec3 g_markers[maxMarkerCount];

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const glm::vec3 g_tileCenterOffset = glm::vec3(0.5f, 0, 0.5f);
const unsigned int g_mapCol = 8;
const unsigned int g_mapRow = 8;

bool g_isFlashLightOn = false;

glm::vec3 g_windowLocations[6];

unsigned int tileMap[][g_mapRow] = {
 	{ 3, 2, 2, 2, 2, 2, 2, 3 },
	{ 1, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 1, 1, 0, 1 },
	{ 1, 0, 0, 0, 1, 1, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 1, 1, 1 },	
};

glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f, 0,  0.0f), 
  glm::vec3( 2.0f, 0, -15.0f), 
  glm::vec3(-1.5f, 0, -2.5f),  
  glm::vec3(-3.8f, 0, -12.3f),  
  glm::vec3( 2.4f, 0, -3.5f),  
  glm::vec3(-1.7f, 0, -7.5f),  
  glm::vec3( 1.3f, 0, -2.5f),  
  glm::vec3( 1.5f, 0, -2.5f), 
  glm::vec3( 1.5f, 0, -1.5f), 
  glm::vec3(-1.3f, 0, -1.5f)  
};

float vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
	0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // top-right
	0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // bottom-right         
	0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // top-right
	-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // bottom-left
	-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // top-left
// Front face
	-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
	0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,  1.0f, 0.0f, // bottom-right
	0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
	0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
	-0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,  0.0f, 1.0f, // top-left
	-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
// Left face
	-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // top-right
	-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-left
	-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-right
	-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // top-right
// Right face
	0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // top-left
	0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // bottom-right
	0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right         
	0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // bottom-right
	0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // top-left
	0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left     
// Bottom face
	-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // top-right
	0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f, // top-left
	0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f, // bottom-left
	0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f, // bottom-left
	-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f, // bottom-right
	-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // top-right
// Top face
	-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top-left
	0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top-right     
	0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
	-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top-left
	-0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f  // bottom-left
};

float quadVertices[] = {  
	// positions   // texCoords
	-0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.0f, 0.0f,
	0.5f, -0.5f,  1.0f, 0.0f,

	-0.5f,  0.5f,  0.0f, 1.0f,
	0.5f, -0.5f,  1.0f, 0.0f,
	0.5f,  0.5f,  1.0f, 1.0f
};	

PointLight pointLights[] = {
	PointLight(glm::vec3(1.0f, -0.4f, 6.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
	PointLight(glm::vec3(1.5f, -0.4f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
	PointLight(glm::vec3(2.0f, -0.4f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	PointLight(glm::vec3(4.0f, -0.4f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
	PointLight(glm::vec3(6.0f, -0.4f, 6.0f), glm::vec3(1.0f, 1.0f, 0.0f)),
};

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if ( window == NULL )
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if ( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_CULL_FACE);
	
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// SHADERS
	// -------
	Shader lightingShader("shaders/color.vs", "shaders/color.fs");
	Shader nanoShader("shaders/test-nano.vs", "shaders/test-nano.fs");
	Shader lampShader("shaders/lamp.vs", "shaders/lamp.fs");
	Shader borderShader("shaders/depth_testing.vs", "shaders/border.fs");
	Shader simpleShader("shaders/depth_testing.vs", "shaders/depth_testing.fs");
	Shader frameBufferShader("shaders/framebuffer.vs", "shaders/framebuffer.fs");
	
	// CONFIGURATION
	// -------------
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Quad vertices
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	// TEXTURES SETUP
	// --------------
	stbi_set_flip_vertically_on_load(true);
	unsigned int diffuseMap = Shader::LoadTextureFromFile("tile.png", "assets");
	unsigned int specularMap = Shader::LoadTextureFromFile("container2_specular.png", "assets/textures");
	unsigned int windowTexture = Shader::LoadTextureFromFile("blending_transparent_window.png", "assets/textures");

	// FRAMEBUFFER SETUP
	// -----------------
	unsigned int frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// LIGHTS SETUP
	// ------------
	unsigned int pointLightCount = sizeof(pointLights) / sizeof(pointLights[0]);
	DirectionLight directionLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, -1.0f, 0.5f));
	directionLight.AmbientIntensity = 0.01f;
	directionLight.DiffuseIntensity = 0.2f;

	SpotLight spotLight(glm::vec3(1.0f), 12.5f, 15.0f);

	// SHADERS SETUP
	// -------------
	lightingShader.use();
	lightingShader.setInt("pointLightCount", pointLightCount);
	lightingShader.setInt("material.texture_diffuse1", 0);
	lightingShader.setInt("material.texture_specular1", 1);

	nanoShader.use();
	nanoShader.setInt("material.texture_diffuse1", 0);
	nanoShader.setInt("material.texture_specular1", 1);
	nanoShader.setFloat("material.shininess", 32.0f);

	borderShader.use();
	borderShader.setInt("texture1", 0); // TODO: We may be able to remove this.
	borderShader.setVec3("color", glm::vec3(0.0f));

	simpleShader.use();
	simpleShader.setInt("texture1", 0);

	// MODELS SETUP
	// ------------
	Model planet("assets/planet/planet.obj");
	Model nanosuit("assets/nanosuit/nanosuit.obj");

	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		// --------------------------------
		// Drawing
		// --------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);//0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // | GL_STENCIL_BUFFER_BIT );
		glEnable(GL_DEPTH_TEST);
		// glStencilMask(0x00);

		glm::mat4 projection = glm::perspective(glm::radians(g_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = g_camera.GetViewMatrix();
		
		// Shaders setup
		lightingShader.use();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setVec3("viewPos", g_camera.Position);
		
		nanoShader.use();
		nanoShader.setMat4("projection", projection);
		nanoShader.setMat4("view", view);
		nanoShader.setVec3("viewPos", g_camera.Position);

		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		simpleShader.use();
		simpleShader.setMat4("projection", projection);
		simpleShader.setMat4("view", view);

		borderShader.use();
		borderShader.setMat4("projection", projection);
		borderShader.setMat4("view", view);

		setupLights(&lampShader, &lightingShader, &directionLight, pointLights, pointLightCount, &spotLight, VAO);

		displayMap(&lightingShader, VAO, diffuseMap, specularMap);
		displayNanosuit(&nanosuit, &nanoShader);
		// displayPlanet(&planet, &lightingShader, &borderShader);
		displayWindows(&simpleShader, quadVAO, windowTexture);

		// Default buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		frameBufferShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0, 0.5f, 0));
		model = glm::scale(model, glm::vec3(2.0f));
		frameBufferShader.setMat4("model", model);

		glBindVertexArray(quadVAO);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);  

 		glfwSwapBuffers(window);
		glfwPollEvents();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// glDeleteBuffers(1, &EBO);
	
	glfwTerminate();
  return 0;
}

void displayNanosuit(Model *nanosuit, Shader *shader)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, -0.5f, 1.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

	shader->use();
	shader->setMat4("model", model);
		
	nanosuit->Draw(*shader);
}

void displayPlanet(Model *planet, Shader *planetShader, Shader *borderShader)
{
	// TODO: Planet should not have specular. Add ability to turn on and off
	// We freeze the stencil
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
		
	// Draw the planet
	drawPlanet(planet, planetShader, 1.0f);

	// We disable the stencil
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	// Draw a bigger planet
	drawPlanet(planet, borderShader, 1.05f);
		
	// Return stencil to normal
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);
}

void drawPlanet(Model *planet, Shader *shader, float scale)
{
	shader->use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.0f, 0.5f, -2.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f) * scale);
	model = glm::rotate(model, (float)glfwGetTime() / 2, glm::vec3(0.3f, 1.0f, 0.0f));
	shader->setMat4("model", model);
	planet->Draw(*shader);
}

void setupLights(Shader *lampShader, Shader *objectShader, DirectionLight *directionLight, PointLight pointLights[], unsigned int pointLightCount, SpotLight *spotLight, unsigned int VAO)
{
	objectShader->use();
	directionLight->setup(objectShader, "dirLight");

	if ( g_isFlashLightOn )
	{
		spotLight->Position = g_camera.Position;
		spotLight->Direction = g_camera.Front;
		spotLight->AmbientIntensity = 0.1f;
		spotLight->DiffuseIntensity = 0.8f;
		spotLight->setup(objectShader, "spotLight");
	}
	else
	{
		objectShader->setBool("isSpotLightSetup", false);
	}
	
	for ( unsigned int i = 0; i < pointLightCount ; ++i )
	{
		pointLights[i].use(objectShader, "pointLights[" + std::to_string(i) + "]");
		pointLights[i].setup(VAO);
		pointLights[i].draw(lampShader);
	}
}

void displayWindows(Shader *shader, unsigned int quadVAO, unsigned int texture)
{
	shader->use();
	
	// Sorting the windows
	// -------------------
	std::map<float, glm::vec3> sorted;
	for (unsigned int i = 0 ; i < 6; i++ )
	{
		float distance = glm::length(g_camera.Position - g_windowLocations[i]);
		sorted[distance] = g_windowLocations[i];
	}
	
	glm::mat4 model;

	for ( std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		model= glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(it->second.x, it->second.y, it->second.z + 0.52f));
	
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, texture);
	
		shader->setMat4("model", model);
	
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glBindVertexArray(0);
}

void displayMap(Shader *shader, unsigned int VAO, unsigned int diffuseMap, unsigned int specularMap)
{
	shader->use();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	unsigned int displayWindowIndex = 0;
	glm::vec3 position;
	for(unsigned int row = 0; row < g_mapRow ; row++)
	{
		for (unsigned int col = 0; col < g_mapCol ; col++)
		{
			float yPos = 0.0f;
			unsigned int tile = tileMap[row][col];
			if ( tile <= 0 )
			{
				//continue;
				yPos = -1.0f;
			}
			else if ( tile == 2 )
			{
				g_windowLocations[displayWindowIndex] = glm::vec3(col, 0.0f, row);
				displayWindowIndex++;
				continue;
			}
			else if ( tile > 2 )
			{
				continue;
			}

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(col, yPos, row));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			shader->setMat4("model", model);

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			glBindVertexArray(0);
		}
	}

	// Markers
	for ( unsigned int index = 0 ; index < maxMarkerCount ; ++index )
	{
 		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(g_markers[index].x, -0.5f, g_markers[index].z));
		model = glm::scale(model, glm::vec3(0.04f, 0.1f, 0.04f));
		int modelLoc = glGetUniformLocation(shader->ID, "model");
		shader->setVec4("tint", 1.0f, 0.0f, 0.0f, 1.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool canMoveToPosition(glm::vec3 currentPosition)
{
	glm::vec3 tileCoordinate = getTileCoords(currentPosition, g_tileCenterOffset);
	if ( tileCoordinate.x < 0 || tileCoordinate.z < 0 )
		return false;
	
	unsigned int tile = tileMap[unsigned(tileCoordinate.z)][unsigned(tileCoordinate.x)];
	if ( tile <= 0 )
		return true;
			
	if ( (currentPosition.x > tileCoordinate.x - g_tileCenterOffset.x && currentPosition.x < tileCoordinate.x + g_tileCenterOffset.x)
 			 && (currentPosition.z > tileCoordinate.z - g_tileCenterOffset.z && currentPosition.z < tileCoordinate.z + g_tileCenterOffset.z )) {
		return false;
	}

	return true;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	glm::vec3 currentPosition = g_camera.Position;
	float movementSpeed = 2.5f;
	float stepDistance = deltaTime * movementSpeed;	
	bool keyPressed = false;
	float playerPadding = 0.25f;
	glm::vec3 direction = glm::vec3(0.0);

	if ( glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
	{
		direction += g_camera.GetForward();
		keyPressed = true;
	}
	else if ( glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		direction += -g_camera.GetForward();
		keyPressed = true;
	}

	if ( glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS)
	{
		direction += -g_camera.Right;
		keyPressed = true;
	}
	else if ( glfwGetKey(window, GLFW_KEY_E ) == GLFW_PRESS)
	{
		direction += g_camera.Right;
		keyPressed = true;
	}

	if ( keyPressed )
	{
		direction = glm::normalize(direction);
		
		glm::vec3 playerCoord = getTileCoords(currentPosition, g_tileCenterOffset);
		
		// We manipulate the position
		currentPosition += direction * stepDistance;

		// We then check if there are any collissions
		glm::vec3 tileAtRightPos = glm::vec3(playerCoord.x + 1, 0, playerCoord.z);
		float tileAtRight = getTileAt(unsigned(tileAtRightPos.x), unsigned(tileAtRightPos.z));
		if ( direction.x > 0 && tileAtRight > 0 ) // There is a tile
		{
			float i = currentPosition.x + playerPadding;
			float j = tileAtRightPos.x - g_tileCenterOffset.x;
			if ( i > j )
			{
				currentPosition.x -= (i - j);
			}
		}
		else
		{
			glm::vec3 tileAtLeftPos = glm::vec3(playerCoord.x - 1, 0, playerCoord.z);
			float tileAtLeft = getTileAt(unsigned(tileAtLeftPos.x), unsigned(tileAtLeftPos.z));
			if ( direction.x < 0 && tileAtLeft > 0 )
			{
				float i = currentPosition.x - playerPadding;
				float j = tileAtLeftPos.x + g_tileCenterOffset.x;
				if ( i < j )
				{
					currentPosition.x += (j - i);
				}	
			}
		}

		glm::vec3 tileAtTopPos = glm::vec3(playerCoord.x, 0, playerCoord.z - 1);
		float tileAtTop = getTileAt(unsigned(tileAtTopPos.x), unsigned(tileAtTopPos.z));
		if ( direction.z < 0 && tileAtTop > 0 ) // There is a tile
		{
			float i = currentPosition.z - playerPadding;
			float j = tileAtTopPos.z + g_tileCenterOffset.z;
			if ( i < j )
			{
				currentPosition.z += (j - i);
			}
		}
		else
		{
			glm::vec3 tileAtBottomPos = glm::vec3(playerCoord.x, 0, playerCoord.z + 1);
			float tileAtBottom = getTileAt(unsigned(tileAtBottomPos.x), unsigned(tileAtBottomPos.z));
			if ( direction.z > 0 && tileAtBottom > 0 )
			{
				float i = currentPosition.z + playerPadding;
				float j = tileAtBottomPos.z - g_tileCenterOffset.z;
				if ( i > j )
				{
					currentPosition.z -= (i - j);
				}	
			}
		}

		if ( canMoveToPosition(currentPosition )) // This is a final check to see if we are somewhere we are not supposed to
			g_camera.UpdatePosition(currentPosition);
	}

	// TODO: Improve this. Should log the lastKeyPressed regardless of what it is.
	// OR. Make an input handler class.
	if ( glfwGetKey(window, GLFW_KEY_PERIOD ) == GLFW_PRESS)
	{
		g_lastKeyPressed = GLFW_KEY_PERIOD;
	}
	else if ( g_lastKeyPressed == GLFW_KEY_PERIOD && glfwGetKey(window, GLFW_KEY_PERIOD ) == GLFW_RELEASE)
	{
		castRay(currentPosition, g_camera.Front, 5);
		g_lastKeyPressed = 0;
	}

	if ( glfwGetKey(window, GLFW_KEY_F ) == GLFW_PRESS)
	{
		g_lastKeyPressed = GLFW_KEY_F;
	}
	else if ( g_lastKeyPressed == GLFW_KEY_F && glfwGetKey(window, GLFW_KEY_F ) == GLFW_RELEASE)
	{
		if(g_isFlashLightOn )
			g_isFlashLightOn = false;
		else
			g_isFlashLightOn = true;
		g_lastKeyPressed = 0;
	}
}

glm::vec3 getTileCoords(glm::vec3 currentPosition, glm::vec3 centerOffset)
{
	glm::vec3 tileCoordinate;
	tileCoordinate.x = floor((currentPosition.x + centerOffset.x) / 1.0f);
	tileCoordinate.z = floor((currentPosition.z + centerOffset.z) / 1.0f);
	return tileCoordinate;
}

glm::vec3 castRay(glm::vec3 startPosition, glm::vec3 rayDirection, float castDistance)
{
	// This link has helped me a lot in making this:
	// https://theshoemaker.de/2016/02/ray-casting-in-2d-grids/
	const float tileSize = 1.0f;

	for ( unsigned int i = 0 ; i < maxMarkerCount ; ++i )
	{
		g_markers[i] = g_camera.Position;
	}
	glm::vec3 currentPosition = startPosition;
	glm::vec3 tileCoordinate = getTileCoords(currentPosition, g_tileCenterOffset);

	int8_t dirSignX = rayDirection.x > 0 ? 1: -1;
	int8_t dirSignZ = rayDirection.z > 0 ? 1: -1;
	int8_t tileOffsetX = rayDirection.x > 0 ? 0: -1;
	int8_t tileOffsetZ = rayDirection.z > 0 ? 0: -1;
	
	unsigned int index = 0;
	float t = 0;
	
	while( index < maxMarkerCount )
	{
		float dtX = ((tileCoordinate.x + 1 + tileOffsetX) - (currentPosition.x + g_tileCenterOffset.x)) / rayDirection.x;
		float dtZ = ((tileCoordinate.z + 1 + tileOffsetZ) - (currentPosition.z + g_tileCenterOffset.z)) / rayDirection.z;

		if ( dtX < dtZ )
		{
			t = t + dtX + 0.001;
			tileCoordinate.x = tileCoordinate.x + dirSignX;
		}
		else
		{
			t = t + dtZ + 0.001;
			tileCoordinate.z = tileCoordinate.z + dirSignZ;
		}

		float rayLength = glm::length(rayDirection * t);
 		if ( rayLength > castDistance ) {
			return glm::vec3();
		}
		g_markers[index] = currentPosition = startPosition + (rayDirection * t);

		glm::vec3 raycastHit = handleObjectAtPos(g_markers[index]);
		if ( glm::any(glm::notEqual(raycastHit, glm::vec3())) )
			return raycastHit;

		index++;
	}
	return glm::vec3();
}

unsigned int getTileAt(unsigned int col, unsigned int row)
{
	if ( col >= g_mapCol || row >= g_mapRow )
		return 0;
	
	return tileMap[row][col];
}

void setTileAt(unsigned int col, unsigned int row, unsigned int value)
{
	tileMap[row][col] = value;
}

glm::vec3 handleObjectAtPos(glm::vec3 raycastPosition)
{
	glm::vec3 tileCoordinate = getTileCoords(raycastPosition, g_tileCenterOffset);
	unsigned int tile = getTileAt(unsigned(tileCoordinate.x),unsigned(tileCoordinate.z));
	if ( tile > 0 )
	{
		//setTileAt(unsigned(tileCoordinate.x),unsigned(tileCoordinate.z), 0);
		return tileCoordinate;
	}
	return glm::vec3();
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if ( g_firstMouse )
	{
		lastX = xPos;
		lastY = yPos;
		g_firstMouse = false;
	}
	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;
	
	g_camera.ProcessMouseMovement(xOffset, yOffset, true);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	g_camera.ProcessMouseScroll(yOffset);
}

// HELPERS ====
void getPositionFromTileIndex(unsigned int index, glm::vec3 *position)
{
	position->z = floor(index / g_mapCol);
	position->y = 0;
	position->x = index - (position->z * g_mapCol);
}
