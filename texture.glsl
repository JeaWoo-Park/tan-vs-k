#version 330 core

in vec3 passColor;
in vec2 Text;

out vec4 FragColor;


uniform sampler2D text;

void main()
{ 

	FragColor =  texture(text, Text);

}