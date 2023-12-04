#pragma once

#include <XRFrameworkBase/XRTexture.h>

class XRTextureGL : public XRTextureRHI
{
private:
	GLuint _texture;

public:
	XRTextureGL(XRRenderDevice* ownerRenderDevice) : XRTextureRHI(ownerRenderDevice) {}
	virtual ~XRTextureGL();

public:
	void upload(XRTextureData const* data);
};