#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#include "Libraries/stb_image.h"
//#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shaders/shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 4.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float rotation = 180.0f;
float shininess = 32.0f;

Shader shaders[3] = {};
int currentShader = 2;



void initGlfw() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

GLFWwindow* createWindow() {
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Graphics", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    return window;
}

bool initGlad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	else return true;
}

glm::vec3 pointLightPositions[] = {
//		glm::vec3( 0.7f,  5.2f,  2.0f),
//		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-2.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};

void renderLighting(Shader shader) {
	
	shader.use();
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("ambientLight", 0.1f, 0.12f, 0.15f);
	shader.setFloat("material.shininess", shininess);
	shader.setFloat("roughness", 1-(shininess/100));
	
	cout << "shininess: " << shininess <<endl;
	cout << "roughness: " << 1-(shininess/100) <<endl;

	shader.setFloat("material.fresnel", 0.8f);
	shader.setInt("material.diffuse", 0);
	shader.setInt("material.specular", 0);
	
	// point light 1
	shader.setVec3("pointLights[0].position", pointLightPositions[0]);
	shader.setVec3("pointLights[0].ambient", 0.15f, 0.15f, 0.15f);
	shader.setVec3("pointLights[0].diffuse", 1.5f, 1.5f, 1.5f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09f);
	shader.setFloat("pointLights[0].quadratic", 0.032f);
	// point light 2
	shader.setVec3("pointLights[1].position", pointLightPositions[1]);
	shader.setVec3("pointLights[1].ambient", 0.05f, 0.02f, 0.05f);
	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.4f, 0.8f);
	shader.setVec3("pointLights[1].specular", 1.0f, 0.50f, 1.0f);
	shader.setFloat("pointLights[1].constant", 1.0f);
	shader.setFloat("pointLights[1].linear", 0.09f);
	shader.setFloat("pointLights[1].quadratic", 0.032f);
}

void render(Shader shader, Model model) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// don't forget to enable shader before setting uniforms
//	shader.use();
	renderLighting(shader);

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	// render the loaded model
	glm::mat4 modelTransform = glm::mat4(1.0f);
	
	modelTransform = glm::translate(modelTransform, glm::vec3(0.0f, -2.0f, -10.0f)); // translate it down so it's at the center of the scene
	modelTransform = glm::rotate(modelTransform, glm::radians(rotation), glm::vec3(0, 1, 0)); // translate it down so it's at the center of the scene
	modelTransform = glm::scale(modelTransform, glm::vec3(50.0f, 50.0f, 50.0f));    // it's a bit too big for our scene, so scale it down
	shader.setMat4("model", modelTransform);
	model.Draw(shader);
}

void updateTime() {
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

GLFWwindow* setup() {
	initGlfw();
	
	GLFWwindow* window = createWindow();
	if (window == NULL) return NULL;

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!initGlad()) return NULL;

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(false);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	
	return window;
}

void updateRotation() {
	rotation += 15.0f * deltaTime;
}

void updateShininess() {
	shininess += 10.0f * deltaTime;
	if (shininess > 100) {
		shininess = 0.0f;
	}
}

void updateCurrentShader(int dir) {
//	currentShader += dir;
	currentShader = (currentShader + dir) % 3;
	cout << currentShader << endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		updateCurrentShader(1);
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		updateCurrentShader(-1);
	}
}

int main()
{
	GLFWwindow* window = setup();
	if (window == NULL) return -1;

    // build and compile shaders
//    Shader shader("Shaders/model_loading_vs", "Shaders/model_loading_fs");
//	Shader lightingShader("Shaders/lighting_shader_vs", "Shaders/cook_torrance_fs.glsl");
	Shader shaders[3] = {
		Shader("Shaders/lighting_shader_vs", "Shaders/blinn_phong_fs.glsl"),
		Shader("Shaders/lighting_shader_vs", "Shaders/cook_torrance_fs.glsl"),
		Shader("Shaders/lighting_shader_vs", "Shaders/toon_fs.glsl"),
	};
//	currentShader = 0;
    // load models
    Model model("Resources/scene.obj");
	
	shaders[currentShader].use();
	
	shaders[currentShader].setInt("material.diffuse", 0);
	shaders[currentShader].setInt("material.specular", 0);
	shaders[currentShader].setFloat("material.fresnel", 0.8f);
	
	glfwSetKeyCallback(window, key_callback);


    // render loop
    while (!glfwWindowShouldClose(window))
    {
		updateTime();
		updateRotation();
		updateShininess();

        processInput(window);

		render(shaders[currentShader], model);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
//		updateCurrentShader(-1);
//	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
//		updateCurrentShader(1);
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
