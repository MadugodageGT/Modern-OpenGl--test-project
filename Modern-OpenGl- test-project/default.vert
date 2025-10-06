#version 330 core

//Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aColor;
//texture
layout (location = 2) in vec2 aTex;
//normal
layout (location = 3) in vec3 aNormal;

// Outputs the color for the Fragment Shader
out vec3 color;
// outputs the texture coordinates to the fragment shader
out vec2 texCoord;
// outputs the normal to the fragment shader
out	vec3 Normal;
//output current position
out vec3 crntPos;

uniform mat4 camMatrix;
uniform mat4 model;



void main()
{
	
	crntPos = vec3(model* vec4(aPos, 1.0f));
	gl_Position = camMatrix * vec4(crntPos, 1.0);

	color = aColor;

	texCoord = aTex;

	Normal = aNormal;
}