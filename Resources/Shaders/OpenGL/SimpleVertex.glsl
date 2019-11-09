#version 400 core

#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

layout (std140, column_major) uniform MatrixBlock
{
	mat4 view;
	mat4 proj;
	mat4 viewProj;
};

struct Material {
    vec4 diffuse;
};

uniform Material Materials;

struct Light {
	vec4 position;
	vec3 intensity;
	float attenuation;
};

layout (shared) uniform LightBlock
//layout (std140) uniform LightBlock
{
	Light light;
};

//uniform vec4 test;

out vec3 frag_normal;

void main()
{
	gl_Position = proj * view * position;

	frag_normal = mat3(view) * normal;
	//frag_normal = mat3(view) * normal + test.xyz;
}
