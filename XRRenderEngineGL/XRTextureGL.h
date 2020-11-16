#pragma once

#include <XRFrameworkBase/XRTexture.h>

class XRTextureGL : public XRTextureRHI
{
private:
	GLuint _texture;

public:
	XRTextureGL() {}
	virtual ~XRTextureGL();

public:
	void upload(XRTextureData const* data);
};