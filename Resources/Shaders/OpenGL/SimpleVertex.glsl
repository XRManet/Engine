#version 440 core

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_include : require

#include "XRCommon.glsl"

#include "XRResObject1.glsl"

// Vertex0 Layout
// p: position, n: normal, d: dummy
// [0] [1] [2] [3]
// p.x p.y p.z d.0
// n.x n.y n.z d.1
layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;

layout(location = 0) out Fragment
{
	vec3 _position;
	vec3 _normal;
} _vertexOut;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	gl_Position = _viewProj * vec4(_position, 1.0);
	
	_vertexOut._position = gl_Position.xyz;
	_vertexOut._normal = mat3(_view) * _normal;
	//_vertexOut._normal = mat3(_view) * _vertexIn._normal + test.xyz;
}
