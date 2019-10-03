#pragma once

#include <XRFrameworkBase/XRTexture.h>

class XRTextureGL : public XRTexture
{
private:
	GLuint _texture;

public:
	XRTextureGL(XRTextureData const* data);
	virtual ~XRTextureGL();
};