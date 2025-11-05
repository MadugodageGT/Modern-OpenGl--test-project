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
	Mesh* droneMesh;  // Use pointer instead
	glm::vec4 droneColor;
	glm::vec3 dronePos;
	glm::mat4 droneModel;

	int i;
	float lastUpdateTime;
	float updateInterval;

	std::vector<pathPoints> paths;


public:
	Drone(Shader& shader);
	~Drone();  // Add destructor to clean up pointer

	void update(float time);
	void draw(Shader& shader, Camera& camera);
	void setPathData(std::string filePath);
};

#endif