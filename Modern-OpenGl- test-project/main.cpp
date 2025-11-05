#include"Mesh.h"
#include "Drone.h"
#include "Grid.h"  // Add this

//window size variables
const unsigned int width = 800;
const unsigned int height = 800;

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

//vertice coordinates for light cub
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

int main()
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "testing-window", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, width, height);

	// Create shaders
	Shader lightShader("light.vert", "light.frag");
	Shader gridShader("grid.vert", "grid.frag");  // Add grid shader

	// Create grid (size = 20 units, 20 divisions)
	Grid grid(20.0f, 20);

	// Create drones
	std::vector <Drone> drones;
	int droneCount = 24;
	drones.reserve(droneCount);

	std::cout << "Creating " << droneCount << " drones..." << std::endl;
	for (int j = 0; j < droneCount; j++) {
		try {
			drones.emplace_back(lightShader);
			std::string pathFile = "path_data_2/Drone " + std::to_string(j + 1) + ".txt";
			drones.back().setPathData(pathFile);
		}
		catch (const std::exception& e) {
			std::cerr << "Error creating drone " << (j + 1) << ": " << e.what() << std::endl;
		}
	}
	std::cout << "All drones created. Starting render loop..." << std::endl;

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(0.0f, 20.0f, 50.0f)); // Further back

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update drones
		for (auto& d : drones) {
			d.update((float)glfwGetTime());
		}

		// Update camera
		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		// Draw grid and axes
		//grid.Draw(gridShader, camera);

		// Draw drones
		for (auto& d : drones) {
			d.draw(lightShader, camera);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	lightShader.Delete();
	gridShader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}