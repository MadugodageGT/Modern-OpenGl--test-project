#include "Drone.h"

Drone::Drone(Shader& shader)
	: currentIndex(0), nextIndex(1), lastUpdateTime(0.0f),
	updateInterval(0.25f), interpolationFactor(0.0f),  // 250ms = 0.25s
	droneColor(1.0f, 1.0f, 1.0f, 1.0f),
	dronePos(0.0f, 0.5f, 0.0f),
	droneModel(1.0f)
{
	// Generate sphere vertices
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	float radius = 0.15f;  // Sphere radius
	int sectorCount = 20;  // Longitude divisions
	int stackCount = 20;   // Latitude divisions

	float x, y, z, xy;
	float nx, ny, nz;
	float s, t;

	float sectorStep = 2 * 3.14159265359f / sectorCount;
	float stackStep = 3.14159265359f / stackCount;
	float sectorAngle, stackAngle;

	// Generate vertices
	for (int i = 0; i <= stackCount; ++i) {
		stackAngle = 3.14159265359f / 2 - i * stackStep;  // Starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);         // r * cos(u)
		z = radius * sinf(stackAngle);          // r * sin(u)

		for (int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep;       // Starting from 0 to 2pi

			// Vertex position (x, y, z)
			x = xy * cosf(sectorAngle);         // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);         // r * cos(u) * sin(v)

			// Normalized vertex normal (nx, ny, nz)
			nx = x / radius;
			ny = y / radius;
			nz = z / radius;

			// Vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectorCount;
			t = (float)i / stackCount;

			Vertex vertex;
			vertex.position = glm::vec3(x, y, z);
			vertex.normal = glm::vec3(nx, ny, nz);
			vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);
			vertex.texUV = glm::vec2(s, t);

			vertices.push_back(vertex);
		}
	}

	// Generate indices
	int k1, k2;
	for (int i = 0; i < stackCount; ++i) {
		k1 = i * (sectorCount + 1);     // Beginning of current stack
		k2 = k1 + sectorCount + 1;      // Beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			// 2 triangles per sector excluding first and last stacks
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (stackCount - 1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	// Textures (can be empty or provide default textures)
	Texture textures[] = {
		Texture("wooden_gate_diff_1k.jpg", "diffuse", 0, GL_RGB, GL_UNSIGNED_BYTE),
		Texture("wooden_gate_rough_1k.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	std::vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));

	// Create mesh using pointer
	droneMesh = new Mesh(vertices, indices, tex);

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
			iss >> V.x >> V.y >> V.z >> V.r >> V.g >> V.b;
			paths.push_back(V);
		}
		std::cout << "Path data loaded successfully! " << paths.size() << " points." << std::endl;
		file.close();

		// Set initial position immediately after loading path data
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
	if (paths.size() < 2) return;  // Need at least 2 points for interpolation

	// Calculate time since last waypoint change
	float timeSinceUpdate = time - lastUpdateTime;

	// Update interpolation factor (0.0 to 1.0)
	interpolationFactor = timeSinceUpdate / updateInterval;

	// Move to next waypoint when interpolation is complete
	if (interpolationFactor >= 1.0f) {
		currentIndex++;
		nextIndex++;

		// Loop back to start
		if (nextIndex >= paths.size()) {
			currentIndex = 0;
			nextIndex = 1;
		}
		else if (currentIndex >= paths.size()) {
			currentIndex = paths.size() - 1;
			nextIndex = 0;
		}

		lastUpdateTime = time;
		interpolationFactor = 0.0f;
	}

	// Clamp interpolation factor to [0, 1]
	interpolationFactor = glm::clamp(interpolationFactor, 0.0f, 1.0f);

	// Get current and next waypoints
	const pathPoints& current = paths[currentIndex];
	const pathPoints& next = paths[nextIndex];

	// Interpolate position (with y/z swap)
	glm::vec3 currentPos(current.x, current.z, current.y);
	glm::vec3 nextPos(next.x, next.z, next.y);
	dronePos = lerp(currentPos, nextPos, interpolationFactor);

	// Interpolate color
	glm::vec3 color = lerpColor(current.r, current.g, current.b,
		next.r, next.g, next.b,
		interpolationFactor);
	droneColor.r = color.r;
	droneColor.g = color.g;
	droneColor.b = color.b;

	// Update model matrix
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