#version 420 core

#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

layout (binding = 0, column_major) uniform MatrixBlock
{
	mat4 view;
	mat4 viewProj;
} transform;

struct Light {
	vec4 position;
	vec3 intensity;
	float attenuation;
};

struct Material {
	vec4 diffuse;
};

uniform Light light;

uniform Material matereials;


out vec3 frag_normal;

void main()
{
	gl_Position = transform.viewProj * position;

	frag_normal = mat3(transform.view) * normal;

	
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
