#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "src/graphics/shader.h"
#include "src/graphics/camera.h"
#include "src/graphics/model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader boxShader("shaders/test-box.vs", "shaders/test-box.fs");
	Shader simpleShader("shaders/simple.vs", "shaders/simple.fs");
	Shader nanoShader("shaders/test-nano.vs", "shaders/test-nano.fs");
	Shader lampShader("shaders/lamp.vs", "shaders/lamp.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	// positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = Shader::LoadTextureFromFile("container2.png", "assets");
	unsigned int specularMap = Shader::LoadTextureFromFile("container2_specular.png", "assets");

	Model cyborg("assets/cyborg/cyborg.obj");
	Model nanosuit("assets/nanosuit/nanosuit.obj");
	Model robot("assets/robot/robot.obj");

	// shader configuration
	// --------------------
	boxShader.use();
	boxShader.setInt("material.diffuse", 0);
	boxShader.setInt("material.specular", 1);

	nanoShader.use();
	nanoShader.setInt("material.diffuse", 0);
	nanoShader.setInt("material.specular", 1);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		boxShader.use();
		boxShader.setVec3("viewPos", camera.Position);
		boxShader.setFloat("material.shininess", 32.0f);

		/*
			Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
			the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
			by defining light types as classes and set their values in there, or by using a more efficient uniform approach
			by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		boxShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		boxShader.setVec3("dirLight.ambient", glm::vec3(0.01f));
		boxShader.setVec3("dirLight.diffuse", glm::vec3(0.05f));
		boxShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		boxShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		boxShader.setVec3("pointLights[0].ambient", 1.05f, 0.05f, 0.05f);
		boxShader.setVec3("pointLights[0].diffuse", 1.8f, 0.8f, 0.8f);
		boxShader.setVec3("pointLights[0].specular", 0.0f, 1.0f, 0.0f);
		boxShader.setFloat("pointLights[0].constant", 1.0f);
		boxShader.setFloat("pointLights[0].linear", 0.09);
		boxShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		boxShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		boxShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		boxShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		boxShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		boxShader.setFloat("pointLights[1].constant", 1.0f);
		boxShader.setFloat("pointLights[1].linear", 0.09);
		boxShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		boxShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		boxShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		boxShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		boxShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		boxShader.setFloat("pointLights[2].constant", 1.0f);
		boxShader.setFloat("pointLights[2].linear", 0.09);
		boxShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		boxShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		boxShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		boxShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		boxShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		boxShader.setFloat("pointLights[3].constant", 1.0f);
		boxShader.setFloat("pointLights[3].linear", 0.09);
		boxShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		boxShader.setVec3("spotLight.position", camera.Position);
		boxShader.setVec3("spotLight.direction", camera.Front);
		boxShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		boxShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		boxShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		boxShader.setFloat("spotLight.constant", 1.0f);
		boxShader.setFloat("spotLight.linear", 0.09);
		boxShader.setFloat("spotLight.quadratic", 0.032);
		boxShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		boxShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));     

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		boxShader.setMat4("projection", projection);
		boxShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		boxShader.setMat4("model", model);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		// render containers
		glBindVertexArray(cubeVAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			boxShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Draw the cyborg
		nanoShader.use();

		nanoShader.setVec3("viewPos", camera.Position);
		nanoShader.setFloat("material.shininess", 32.0f);

		nanoShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		nanoShader.setVec3("dirLight.ambient", glm::vec3(0.01f));
		nanoShader.setVec3("dirLight.diffuse", glm::vec3(0.05f));
		nanoShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		// point light 1
		nanoShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		nanoShader.setVec3("pointLights[0].ambient", 1.05f, 0.05f, 0.05f);
		nanoShader.setVec3("pointLights[0].diffuse", 1.8f, 0.8f, 0.8f);
		nanoShader.setVec3("pointLights[0].specular", 0.0f, 1.0f, 0.0f);
		nanoShader.setFloat("pointLights[0].constant", 1.0f);
		nanoShader.setFloat("pointLights[0].linear", 0.09);
		nanoShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		nanoShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		nanoShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		nanoShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		nanoShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		nanoShader.setFloat("pointLights[1].constant", 1.0f);
		nanoShader.setFloat("pointLights[1].linear", 0.09);
		nanoShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		nanoShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		nanoShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		nanoShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		nanoShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		nanoShader.setFloat("pointLights[2].constant", 1.0f);
		nanoShader.setFloat("pointLights[2].linear", 0.09);
		nanoShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		nanoShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		nanoShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		nanoShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		nanoShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		nanoShader.setFloat("pointLights[3].constant", 1.0f);
		nanoShader.setFloat("pointLights[3].linear", 0.09);
		nanoShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		nanoShader.setVec3("spotLight.position", camera.Position);
		nanoShader.setVec3("spotLight.direction", camera.Front);
		nanoShader.setVec3("spotLight.ambient", glm::vec3(0.5f));
		nanoShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		nanoShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		nanoShader.setFloat("spotLight.constant", 1.0f);
		nanoShader.setFloat("spotLight.linear", 0.09);
		nanoShader.setFloat("spotLight.quadratic", 0.032);
		nanoShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		nanoShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));     

		nanoShader.setMat4("projection", projection);
		nanoShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		nanoShader.setMat4("model", model);
		cyborg.Draw(nanoShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f));
		nanoShader.setMat4("model", model);
		nanosuit.Draw(nanoShader);

		// simpleShader.use();
		// simpleShader.setMat4("projection", projection);
		// simpleShader.setMat4("view", view);

		// Draw the robot
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		nanoShader.setMat4("model", model);
		robot.Draw(nanoShader);
		
		// also draw the lamp object(s)
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		lampShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    
		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lampShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
