#pragma once

#include <XRFrameworkBase/XRTexture.h>

class XRTextureVK : public XRTextureRHI
{
private:
	VkImage _texture;

public:
	XRTextureVK() {}
	virtual ~XRTextureVK();

public:
	void upload(XRTextureData const* data);
};