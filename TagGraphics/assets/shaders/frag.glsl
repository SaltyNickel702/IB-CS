#version 410 core

in vec4 color;

out vec4 FragColor;

void main()
{
	FragColor = color;
	// FragColor = vec4(1,1,1,1);
}