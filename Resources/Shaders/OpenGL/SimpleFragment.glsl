#version 330 core

in vec3 frag_position;
in vec3 frag_normal;

out vec3 color;

void main()
{
	color = frag_position;
	//color = frag_position * frag_normal;
}