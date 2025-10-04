#version 330 core

//Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aColor;
//texture
layout (location = 2) in vec2 aTex;


// Outputs the color for the Fragment Shader
out vec3 color;
// outputs the texture coordinates to the fragment shader
out vec2 texCoord;


// Controls the scale of the vertices
uniform float scale;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;



void main()
{
	
	gl_Position = proj * view * model * vec4(aPos, 1.0);

	color = aColor;

	texCoord = aTex;

}