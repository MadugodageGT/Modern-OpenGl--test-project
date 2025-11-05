#include "Drone.h"

Drone::Drone(Shader& shader)
	: i(0), lastUpdateTime(0.0f), updateInterval(0.025f),
	droneColor(1.0f, 1.0f, 1.0f, 1.0f),
	dronePos(0.0f, 0.5f, 0.0f),
	droneModel(1.0f)
{
	// Vertices for drone cube
	Vertex vertices[] = {
		Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
		Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
		Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
		Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
		Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
	};

	GLuint indices[] = {
		0, 1, 2,  0, 2, 3,
		0, 4, 7,  0, 7, 3,
		3, 7, 6,  3, 6, 2,
		2, 6, 5,  2, 5, 1,
		1, 5, 4,  1, 4, 0,
		4, 5, 6,  4, 6, 7
	};

	// Textures (temporary)
	Texture textures[] = {
		Texture("wooden_gate_diff_1k.jpg", "diffuse", 0, GL_RGB, GL_UNSIGNED_BYTE),
		Texture("wooden_gate_rough_1k.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	std::vector<Vertex> droneVerts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint> droneInds(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));

	// Create mesh using pointer
	droneMesh = new Mesh(droneVerts, droneInds, tex);

	droneModel = glm::translate(droneModel, dronePos);
	shader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(droneModel));
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), droneColor.x, droneColor.y, droneColor.z, droneColor.w);
}

Drone::~Drone() {
	delete droneMesh;
}

void Drone::setPathData(std::string filePath) {
	std::fstream file;
	file.open(filePath, std::ios::in);
	if (file.is_open()) {
		std::string line;
		std::getline(file, line); // Skip header line

		while (getline(file, line)) {
			std::stringstream iss(line);
			pathPoints V;
			iss >> V.timeStamp >> V.x >> V.y >> V.z >> V.vx >> V.vy >> V.vz >> V.r >> V.g >> V.b;
			paths.push_back(V);
		}
		std::cout << "Path data loaded successfully! " << paths.size() << " points." << std::endl;
		file.close();

		// IMPORTANT: Set initial position immediately after loading path data
		if (!paths.empty()) {
			dronePos.x = paths[0].x;
			dronePos.y = paths[0].z;  // Swapping y and z
			dronePos.z = paths[0].y;
			droneColor.r = paths[0].r;
			droneColor.g = paths[0].g;
			droneColor.b = paths[0].b;

			std::cout << "  Initial position: (" << dronePos.x << ", "
				<< dronePos.y << ", " << dronePos.z << ")" << std::endl;
		}
	}
	else {
		std::cout << "Unable to open file: " << filePath << std::endl;
	}
}

void Drone::update(float time) {
	if (paths.empty()) return;

	if (time - lastUpdateTime >= updateInterval) {
		i++;
		if (i >= paths.size()) i = 0;
		lastUpdateTime = time;
	}

	dronePos.x = paths[i].x;
	dronePos.y = paths[i].z;
	dronePos.z = paths[i].y;
	droneColor.r = paths[i].r;
	droneColor.g = paths[i].g;
	droneColor.b = paths[i].b;

	// Just compute model matrix
	droneModel = glm::mat4(1.0f);
	droneModel = glm::translate(droneModel, dronePos);
}


void Drone::draw(Shader& shader, Camera& camera) {
	shader.Activate();
	camera.Matrix(shader, "camMatrix");

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(droneModel));
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"),
		droneColor.r, droneColor.g, droneColor.b, droneColor.w);

	droneMesh->Draw(shader, camera);
}