#version 330 core

in vec3 frag_normal;

struct Light {
	vec4 position;
	vec3 intensity;
	float attenuation;
};

struct Material {
	vec4 diffuse;
};

uniform Light light;

out vec3 color;

void main()
{
	if(light.position.w == 0)
	{
		light.position - gl_Position;
	}
	
	color = vec3(1, 0, 0);
}