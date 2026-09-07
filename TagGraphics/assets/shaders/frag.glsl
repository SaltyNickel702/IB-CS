#version 410 core

uniform vec3 color;

out vec4 FragColor;

void main()
{
	FragColor = vec4(color,1);
	// FragColor = vec4(1,1,1,1);
}