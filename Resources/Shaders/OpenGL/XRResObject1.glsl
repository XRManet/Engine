#define xrSetObject1	2

layout(XR_SET(xrSetObject1) binding = 0, std140, column_major) uniform MatrixBlock
{
	mat4 _view;
	mat4 _proj;
	mat4 _viewProj;
};

struct Material {
	vec4 _diffuse;
};

layout(XR_SET(xrSetObject1) binding = 1) uniform Materials
{
	Material _material;
};

struct Light {
	vec4 _position;
	vec3 _intensity;
	float _attenuation;
};

layout(XR_SET(xrSetObject1) binding = 2, shared) uniform LightBlock
{
	Light _light;
};

//uniform vec4 test;