#version 410 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 colorIn;

uniform mat4 viewMat;
uniform mat4 screenMat;

out vec4 color;

void main ()
{
	color = vec4(colorIn,1);

	vec4 view = viewMat * vec4(pos,0,1);
	vec4 clip = screenMat * view;
	gl_Position = clip;
}