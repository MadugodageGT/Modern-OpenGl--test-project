#ifndef DRONE_CLASS_H
#define DRONE_CLASS_H

#include "Mesh.h"
Mesh droneMesh;
const Shader droneShader;

Vertex vertices[];
GLuint indices[];

glm::vec4 droneColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec3 dronePos = glm::vec3(0.0f, 0.5f, 0.0f);
glm::mat4 droneModel = glm::mat4(1.0f);


int i = 0;
float lastUpdateTime = 0.0f;
float updateInterval = 0.25f; // 250ms in seconds

struct pathPoints {
	float timeStamp;
	float x, y, z;
	float vx, vy, vz;
	float r, g, b;
};

std::vector <pathPoints> paths;

class Drone {
public:
	
	Drone(Shader& shader);
	void update(float time, Shader& sahder);
	void draw(Shader& shader, Camera& camera);
	void setPathData(std::string filePath);

};


#endif