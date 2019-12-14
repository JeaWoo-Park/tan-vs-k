#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 g_color;
out vec3 passColor;

uniform mat4 transform;

void main()
{

	gl_Position = transform * vec4(in_Position, 1.0f);
	passColor = g_color;
}