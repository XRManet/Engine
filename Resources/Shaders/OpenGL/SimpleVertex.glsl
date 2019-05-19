#version 330 core

#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 transformView;
uniform mat4 transformVP;

out vec3 frag_normal;

void main()
{
	gl_Position = transformVP * position;

	frag_normal = mat3(transformView) * normal;
}
