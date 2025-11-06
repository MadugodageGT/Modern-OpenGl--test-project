#ifndef DRONE_CLASS_H
#define DRONE_CLASS_H
#include "Mesh.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct pathPoints {
	float x, y, z;
	float r, g, b;
};

class Drone {
private:
	Mesh* droneMesh;
	glm::vec4 droneColor;
	glm::vec3 dronePos;
	glm::mat4 droneModel;

	// Interpolation members
	int currentIndex;
	int nextIndex;
	float lastUpdateTime;
	float updateInterval;
	float interpolationFactor;

	std::vector<pathPoints> paths;

	// Helper functions for smooth interpolation
	glm::vec3 lerp(const glm::vec3& start, const glm::vec3& end, float t) {
		return start + t * (end - start);
	}

	glm::vec3 lerpColor(float r1, float g1, float b1, float r2, float g2, float b2, float t) {
		return glm::vec3(
			r1 + t * (r2 - r1),
			g1 + t * (g2 - g1),
			b1 + t * (b2 - b1)
		);
	}

public:
	Drone(Shader& shader);
	~Drone();
	void update(float time);
	void draw(Shader& shader, Camera& camera);
	void setPathData(std::string filePath);
};
#endif