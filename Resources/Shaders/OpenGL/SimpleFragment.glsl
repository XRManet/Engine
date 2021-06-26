#version 330 core

#include "XRCommon.glsl"

#include "XRResFrame.glsl"
#include "XRResObject1.glsl"

layout(location = 0) in Fragment
{
	vec3 _position;
	vec3 _normal;
} _fragmentIn;

layout(location = 0) out Framebuffer
{
	vec3 _color0;
} _framebuffer;

void main()
{
	_framebuffer._color0 = _fragmentIn._position;
	//_framebuffer._color0 = _fragmentIn._position * _fragmentIn._normal;
}