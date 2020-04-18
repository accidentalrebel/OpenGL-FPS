#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "src/graphics/camera.h"
#include "src/graphics/shader.h"
#include "src/graphics/light/light_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow *window);
void displayMap(Shader *shader);
void getPositionFromTileIndex(uint8_t index, glm::vec3 *positions);
bool canMoveToPosition(glm::vec3 currentPosition);
unsigned int loadTexture(const char *path);

glm::vec3 getTileCoords(glm::vec3 currentPosition, glm::vec3 centerOffset);
glm::vec3 castRay(glm::vec3, glm::vec3, float);
glm::vec3 handleObjectAtPos(glm::vec3);
uint8_t getTileAt(uint8_t col, uint8_t row);
void setTileAt(uint8_t col, uint8_t row, uint8_t value);

Camera g_camera(glm::vec3(2.0f, 0.0f, 4.0f));
float lastX = 400.0f, lastY = 300.0f;
float g_lastKeyPressed = 0;
bool g_firstMouse = true;

const uint8_t maxMarkerCount = 10;
glm::vec3 g_markers[maxMarkerCount];

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const glm::vec3 g_tileCenterOffset = glm::vec3(0.5f, 0, 0.5f);
const uint8_t g_mapCol = 8;
const uint8_t g_mapRow = 8;

uint8_t tileMap[][g_mapRow] = {
 	{ 1, 1, 1, 1, 1, 1, 1, 1 },
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
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

// unsigned int indices[] = {
// 	0, 1, 3,
// 	1, 2 ,3
// };

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// --------------------------------
	// Configuration
	// --------------------------------
	// NOTE:
	// Vertex Buffer Objects (VBO) - Can store large number of vertices into the GPUs memory
	// Vertex Array Objects (VAO) - This helps in storing vertex calls so we don't have to do those calls again
	// Element Buffer Objcets (EBO) - Stores indices to help decide which vertices to draw. In the example, it made drawing two adjacent triangles use less vertex points
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

	// TEXTURES AND SHADERS
	stbi_set_flip_vertically_on_load(true);
	unsigned int diffuseMap = loadTexture("assets/tile.png");
	unsigned int specularMap = loadTexture("assets/container2_specular.png");

	Shader lightingShader("shaders/color.vs", "shaders/color.fs");
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	Shader lampShader("shaders/lamp.vs", "shaders/lamp.fs");

	PointLight pointLights[] = {
		PointLight(glm::vec3(2.0f, 0.0f, 2.0f), glm::vec3(1.0f, 0.0f, 0.0f))
	};
	
	// LIGHTS SETUP
	DirectionLight directionLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, -1.0f, 0.5f));
	directionLight.AmbientIntensity = 0.3f;

	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		// --------------------------------
		// Drawing
		// --------------------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.use();
		lightingShader.setVec3("viewPos", g_camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);
		LightUtils::SetupDirectionLight(&directionLight, &lightingShader, "dirLight");
		LightUtils::SetupPointLight(&pointLights[0], &lightingShader, "pointLights[0]");

		glm::mat4 projection = glm::perspective(glm::radians(g_camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::mat4 view = g_camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		displayMap(&lightingShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glBindVertexArray(VAO);

		// LAMP
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		for(unsigned int i = 0; i < 1; i++)
		{
			lampShader.setVec3("lightColor", pointLights[i].Color * 0.8f);
			
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLights[i].Position);
			model = glm::scale(model, glm::vec3(0.2f));
			lampShader.setMat4("model", model);

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

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

void displayMap(Shader *shader)
{
	glm::vec3 position;
	for(uint8_t row = 0; row < g_mapRow ; row++)
	{
		for (uint8_t col = 0; col < g_mapCol ; col++)
		{
			float yPos = 0.0f;
			uint8_t tile = tileMap[col][row];
			if ( tile <= 0 )
			{
				//continue;
				yPos = -1.0f;
			}	

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(col, yPos, row));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			shader->setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	// Markers
	for ( uint8_t index = 0 ; index < maxMarkerCount ; ++index )
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
	
	uint8_t tile = tileMap[unsigned(tileCoordinate.x)][unsigned(tileCoordinate.z)];
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
		glm::vec3 playerCoord = getTileCoords(currentPosition, g_tileCenterOffset);
		
		direction += g_camera.GetForward();
		direction = glm::normalize(direction);

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
	
	// if ( glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
	// {
	// 	direction += g_camera.GetForward();
	// 	glm::vec3 playerCoord = getTileCoords(currentPosition, g_tileCenterOffset);
	// 	glm::vec3 rayTileCoord = castRay(currentPosition, direction, 0.5f);
		
	// 	if ( glm::any(glm::greaterThan(rayTileCoord, glm::vec3(0))) )
	// 	{
	// 		std::cout << "Cannot move further." << rayTileCoord.x << "," << rayTileCoord.z << std::endl;
	// 		std::cout << "Current position." << currentPosition.x << "," << currentPosition.z << std::endl;

	// 		// Check going against north and south wall
	// 		if ( rayTileCoord.x == playerCoord.x ) 
	// 		{
	// 			std::cout << "Cutting z." << std::endl;				
	// 			direction.z = 0;
	// 		}
	// 		// Check sliding against north wall going east
	// 		else if ( rayTileCoord.x + 1 == playerCoord.x
	// 							&& getTileAt(rayTileCoord.x + 1, rayTileCoord.z) > 0 )
	// 		{
	// 			std::cout << "2 >Cutting z." << std::endl;
	// 			direction.z = 0;
	// 		}
	// 		// Check sliding against north wall going west
	// 		else if ( rayTileCoord.x - 1 == playerCoord.x
	// 							&& getTileAt(rayTileCoord.x - 1, rayTileCoord.z) > 0 )
	// 		{
	// 			std::cout << "3 >Cutting z." << std::endl;
	// 			direction.z = 0;
	// 		}
	// 		// Check going against west and east wall
	// 		else if ( rayTileCoord.z == playerCoord.z )
	// 		{
	// 			std::cout << "Cutting x." << std::endl;				
	// 			direction.x = 0;
	// 		}
	// 		// Check sliding against west wall going south
	// 		else if ( rayTileCoord.z - 1 == playerCoord.z
	// 							&& getTileAt(rayTileCoord.x, rayTileCoord.z - 1) > 0 )
	// 		{
	// 			std::cout << "2 > Cutting x." << std::endl;				
	// 			direction.x = 0;
	// 		}
	// 		// Check sliding against west wall going north
	// 		else if ( rayTileCoord.z + 1 == playerCoord.z )
	// 		{
	// 			std::cout << "3 > Cutting x." << std::endl;				
	// 			direction.x = 0;
	// 		}
	// 	}

	// 	if ( direction.x != 0 || direction.z != 0 )
	// 	{
	// 		direction = glm::normalize(direction);

	// 		// TEST
	// 		stepDistance = deltaTime * 0.5f;
	// 		// END
	// 		currentPosition += direction * stepDistance;

	// 		if( canMoveToPosition(currentPosition))
	// 			g_camera.UpdatePosition(currentPosition);
	// 	}
	// }
	else if ( glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		direction += -g_camera.Front;
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
		currentPosition += direction * stepDistance;
		if ( glm::all(glm::equal(castRay(currentPosition, direction, 0.5f), glm::vec3()))
				 && canMoveToPosition(currentPosition))
			g_camera.UpdatePosition(currentPosition);
	}

	if ( glfwGetKey(window, GLFW_KEY_PERIOD ) == GLFW_PRESS)
	{
		g_lastKeyPressed = GLFW_KEY_PERIOD;
	}
	else if ( g_lastKeyPressed == GLFW_KEY_PERIOD && glfwGetKey(window, GLFW_KEY_PERIOD ) == GLFW_RELEASE)
	{
		castRay(currentPosition, g_camera.Front, 5);
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

	for ( uint8_t i = 0 ; i < maxMarkerCount ; ++i )
	{
		g_markers[i] = g_camera.Position;
	}
	glm::vec3 currentPosition = startPosition;
	glm::vec3 tileCoordinate = getTileCoords(currentPosition, g_tileCenterOffset);

	int8_t dirSignX = rayDirection.x > 0 ? 1: -1;
	int8_t dirSignZ = rayDirection.z > 0 ? 1: -1;
	int8_t tileOffsetX = rayDirection.x > 0 ? 0: -1;
	int8_t tileOffsetZ = rayDirection.z > 0 ? 0: -1;
	
	uint8_t index = 0;
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

uint8_t getTileAt(uint8_t col, uint8_t row)
{
	if ( col >= g_mapCol || row >= g_mapRow )
		return 0;
	
	return tileMap[col][row];
}

void setTileAt(uint8_t col, uint8_t row, uint8_t value)
{
	tileMap[col][row] = value;
}

glm::vec3 handleObjectAtPos(glm::vec3 raycastPosition)
{
	glm::vec3 tileCoordinate = getTileCoords(raycastPosition, g_tileCenterOffset);
	uint8_t tile = getTileAt(unsigned(tileCoordinate.x),unsigned(tileCoordinate.z));
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
void getPositionFromTileIndex(uint8_t index, glm::vec3 *position)
{
	position->z = floor(index / g_mapCol);
	position->y = 0;
	position->x = index - (position->z * g_mapCol);
}


unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

