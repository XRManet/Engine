#version 400 core

#extension GL_ARB_separate_shader_objects : enable

#include "XRCommon.glsl"

#include "XRResObject1.glsl"

XR_INPUT_LAYOUT(location = 0) in Vertex0
{
	// Vertex0 Layout
	// p: position, n: normal, d: dummy
	// [0] [1] [2] [3]
	// p.x p.y p.z d.0
	// n.x n.y n.z d.1
	layout (location = 0) vec3 _position;
	layout (location = 1) vec3 _normal;
	layout (location = 0, component = 3) float _dummy[2];
} _vertexIn;

layout(location = 0) out Fragment
{
	vec3 _position;
	vec3 _normal;
} _vertexOut;

void main()
{
	gl_Position = _viewProj * vec4(_vertexIn._position, 1.0);
	
	_vertexOut._position = gl_Position.xyz;
	_vertexOut._normal = mat3(_view) * _vertexIn._normal;
	//_vertexOut._normal = mat3(_view) * _vertexIn._normal + test.xyz;
}
