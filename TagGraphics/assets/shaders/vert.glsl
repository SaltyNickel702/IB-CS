#version 410 core
layout (location = 0) in vec2 pos;

uniform mat4 worldMat;
uniform mat4 viewMat;
uniform mat4 screenMat;

void main ()
{
	vec4 world = worldMat * vec4(pos,0,1);
	vec4 view = viewMat * world;
	vec4 clip = screenMat * view;
	gl_Position = clip;
}