#include "Mesh.h"
#include "Drone.h"
#include "Grid.h"

// Initial window size variables
const unsigned int INITIAL_WIDTH = 1024;
const unsigned int INITIAL_HEIGHT = 800;

// Current window size (will be updated on resize)
int currentWidth = INITIAL_WIDTH;
int currentHeight = INITIAL_HEIGHT;

// Vertices coordinates
Vertex vertices[] =
{ //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

// Vertex coordinates for light cube
Vertex lightVertices[] =
{                //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

// Framebuffer resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	currentWidth = width;
	currentHeight = height;
	glViewport(0, 0, width, height);
}

int main()
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Window hints for proper window creation
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);  // Enable window decorations (title bar, borders)

	// Create window
	GLFWwindow* window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "Drone Simulation - 3D View", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();

	// Set viewport
	glViewport(0, 0, INITIAL_WIDTH, INITIAL_HEIGHT);

	// Register resize callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Create shaders
	Shader droneShader("light.vert", "light.frag");
	Shader gridShader("grid.vert", "grid.frag");

	// Create grid (size = 20 units, 20 divisions)
	Grid grid(500.0f, 50);

	// Create drones
	std::vector<Drone> drones;
	int droneCount = 24;
	drones.reserve(droneCount);

	std::cout << "Creating " << droneCount << " drones..." << std::endl;

	for (int j = 0; j < droneCount; j++) {
		try {
			drones.emplace_back(droneShader);
			std::string pathFile = "path_data_5/Drone " + std::to_string(j + 1) + ".txt";
			drones.back().setPathData(pathFile);
		}
		catch (const std::exception& e) {
			std::cerr << "Error creating drone " << (j + 1) << ": " << e.what() << std::endl;
		}
	}

	std::cout << "All drones created. Starting render loop..." << std::endl;

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Create camera with initial dimensions
	Camera camera(currentWidth, currentHeight, glm::vec3(0.0f, 20.0f, 45.0f));

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update drones
		for (auto& d : drones) {
			d.update((float)glfwGetTime());
		}

		// Update camera with current window dimensions
		// This keeps the aspect ratio correct without scaling objects
		camera.width = currentWidth;
		camera.height = currentHeight;
		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 300.0f);

		// Draw grid and axes
		grid.Draw(gridShader, camera);

		// Draw drones
		for (auto& d : drones) {
			d.draw(droneShader, camera);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	droneShader.Delete();
	gridShader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}