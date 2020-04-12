#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow *window);
void displayMap(Shader *shader);
void getPositionFromTileIndex(uint8_t index, glm::vec3 *positions);
bool canMoveToPosition(glm::vec3 currentPosition);
void getTileCoords(glm::vec3 currentPosition, glm::vec3 centerOffset, glm::vec3 *tileCoordinate);

glm::vec3 castRay(glm::vec3, float);
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
 	{ 1, 1, 1, 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0, 1, 1, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 0 },
	{ 1, 0, 0, 0, 0, 1, 1, 0 },
	{ 1, 0, 1, 0, 0, 1, 1, 0 },
	{ 1, 0, 1, 0, 0, 1, 1, 0 },
	{ 1, 0, 1, 0, 0, 1, 1, 0 },
	{ 1, 1, 1, 0, 0, 0, 0, 0 },	
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
	// Farthest face
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,

		// Nearest face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		// Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

		// Right face
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

		// // Bottom face
    // -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    //  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    // -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    // -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		// Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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

	// Shader
	Shader shader("shader.vs", "shader.fs");

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
	// glGenBuffers(1, &EBO);

	// Any subsequent vertex attribute calls from this point on will be stored inside the VAO
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
	glEnableVertexAttribArray(1);

	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
	// glEnableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// We unbind the VAO vertex array so it is not overwritten
	glBindVertexArray(0);

	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	
	int width, height, nrChannels;
	unsigned char *data = stbi_load("assets/tile.png", &width, &height, &nrChannels, 0);
	if ( data )
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture1" << std::endl;
	}
 	stbi_image_free(data);

	shader.use();
	shader.setInt("texture1", 0);

	// Settings
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		// --------------------------------
		// Drawing
		// --------------------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

		// Rotation and scaling test

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAO);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(g_camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		//projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
		int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 view = g_camera.GetViewMatrix();
		int viewLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		displayMap(&shader);
		
		// for(unsigned int i = 0; i < 10 ; i++)
		// {
		// 	glm::mat4 model = glm::mat4(1.0f);
		// 	model = glm::translate(model, cubePositions[i]);
		// 	float angle = 20.0f * i;
		// 	model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0));
		// 	// float scale = sin((float)i / 10 * glfwGetTime());
		// 	// model = glm::scale(model, glm::vec3(scale, scale, scale));
		// 	int modelLoc = glGetUniformLocation(shader.ID, "model");
		// 	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// 	glDrawArrays(GL_TRIANGLES, 0, 36);
		// }

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
			int modelLoc = glGetUniformLocation(shader->ID, "model");
			shader->setVec4("tint", 1.0f, 1.0f, 1.0f, 1.0f);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	// Markers
	for ( uint8_t index = 0 ; index < maxMarkerCount ; ++index )
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(g_markers[index].x, -0.5f, g_markers[index].z));
		model = glm::scale(model, glm::vec3(0.04f, 0.05f, 0.04f));
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
	glm::vec3 tileCoordinate;
	getTileCoords(currentPosition, g_tileCenterOffset, &tileCoordinate);

	if ( tileCoordinate.x < 0 || tileCoordinate.z < 0 )
		return false;
	
	uint8_t tile = tileMap[unsigned(tileCoordinate.x)][unsigned(tileCoordinate.z)];
	if ( tile <= 0 )
		return true;
			
	if ( (currentPosition.x > tileCoordinate.x - g_tileCenterOffset.x && currentPosition.x < tileCoordinate.z + g_tileCenterOffset.z)
 			 && (currentPosition.z > tileCoordinate.z - g_tileCenterOffset.x && currentPosition.z < tileCoordinate.z + g_tileCenterOffset.z )) {
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
	glm::vec3 direction = glm::vec3(0.0);

	if ( glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
	{
		direction += g_camera.Front;
		glm::vec3 rayHit = castRay(direction, 0.5f);
		if ( glm::any(glm::greaterThan(rayHit, glm::vec3(0))) )
		{
			std::cout << "Cannot move further." << std::endl;
			if ( currentPosition.x > rayHit.x - 0.75f && currentPosition.x < rayHit.x + 0.75f )
			{
				std::cout << "Cutting z." << std::endl;				
				direction.z = 0;
			}
			else if ( currentPosition.z > rayHit.z - 0.75f && currentPosition.z < rayHit.z + 0.75f )
			{
				std::cout << "Cutting x." << std::endl;				
				direction.x = 0;
			}
		}

		direction = glm::normalize(direction);
		currentPosition += direction * stepDistance;

		if( canMoveToPosition(currentPosition))
			g_camera.UpdatePosition(currentPosition);
	}
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
		if ( glm::all(glm::equal(castRay(direction, 0.5f), glm::vec3()))
				 && canMoveToPosition(currentPosition))
			g_camera.UpdatePosition(currentPosition);
	}

	if ( glfwGetKey(window, GLFW_KEY_PERIOD ) == GLFW_PRESS)
	{
		g_lastKeyPressed = GLFW_KEY_PERIOD;
	}
	else if ( g_lastKeyPressed == GLFW_KEY_PERIOD && glfwGetKey(window, GLFW_KEY_PERIOD ) == GLFW_RELEASE)
	{
		castRay(g_camera.Front, 5);
		g_lastKeyPressed = 0;
	}
}

void getTileCoords(glm::vec3 currentPosition, glm::vec3 centerOffset, glm::vec3 *tileCoordinate)
{
	tileCoordinate->x = floor((currentPosition.x + centerOffset.x) / 1.0f);
	tileCoordinate->z = floor((currentPosition.z + centerOffset.z) / 1.0f);
}

glm::vec3 castRay(glm::vec3 rayDirection, float castDistance)
{
	// This link has helped me a lot in making this:
	// https://theshoemaker.de/2016/02/ray-casting-in-2d-grids/
	const float tileSize = 1.0f;

	for ( uint8_t i = 0 ; i < maxMarkerCount ; ++i )
	{
		g_markers[i] = g_camera.Position;
	}
	glm::vec3 startPosition = g_camera.Position;
	glm::vec3 currentPosition = g_camera.Position;
	glm::vec3 tileCoordinate;

	int8_t dirSignX = rayDirection.x > 0 ? 1: -1;
	int8_t dirSignZ = rayDirection.z > 0 ? 1: -1;
	int8_t tileOffsetX = rayDirection.x > 0 ? 0: -1;
	int8_t tileOffsetZ = rayDirection.z > 0 ? 0: -1;

	getTileCoords(currentPosition, g_tileCenterOffset, &tileCoordinate);
	
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
	glm::vec3 tileCoordinate;
	getTileCoords(raycastPosition, g_tileCenterOffset, &tileCoordinate);
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

