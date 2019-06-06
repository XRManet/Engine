#version 430 core

#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

layout (std140, column_major) uniform MatrixBlock
{
	mat4 view;
	mat4 viewProj;
};

struct Light {
	vec4 position;
	vec3 intensity;
	float attenuation;
	float test;
};

layout (shared) uniform LightBlock
{
	Light light;
};

struct Material {
	vec4 diffuse;
};

uniform Material matereials;
uniform vec4 test;

out vec3 frag_normal;

void main()
{
	gl_Position = viewProj * position;

	frag_normal = mat3(view) * normal + test.xyz;

	
//	vec3 normal;
//	if(light.position.w == 0)
//	{
//		normal = -light.position.xyz;
//	}
//	else
//	{
//		normal = light.position.xyz;
//	}
}
