#ifndef DRONE_CLASS_H
#define DRONE_CLASS_H

#include<string>

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"

class Drone
{
public:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	// Store VAO in public so it can be used in the Draw function
	VAO VAO;

	// Initializes the mesh
	Drone(std::vector <Vertex>& vertices, std::vector <GLuint>& indices);

	// Draws the mesh
	void Draw(Shader& shader, Camera& camera);
};
#endif