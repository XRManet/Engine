#version 440 core

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_include : require

#include "XRCommon.glsl"

#include "XRResFrame.glsl"
#include "XRResObject1.glsl"

in Fragment
{
	vec3 _position;
	vec3 _normal;
} _fragmentIn;

layout(location = 0) out vec3 _color0;

void main()
{
	_color0 = _fragmentIn._position;
	//_framebuffer._color0 = _fragmentIn._position * _fragmentIn._normal;
}