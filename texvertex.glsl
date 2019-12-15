#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 g_color;
layout(location = 2) in vec3 aTexcoord;


out vec3 passColor;
out vec2 Text;

uniform mat4 transform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
void main()
{

	gl_Position = projectionTransform * viewTransform * transform * vec4(in_Position, 1.0f);
	passColor = g_color;
	Text = vec2(aTexcoord.x, aTexcoord.y);

}