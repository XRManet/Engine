#pragma once

#include <stdafx.h>

class XRTextureData;

class XRBaseExport XRTexture
{
protected:
	XRTextureData const* _data;

public:
	XRTexture(XRTextureData const* data) : _data(data) {}
	virtual ~XRTexture() {}
};

class XRTextureAtlas : public XRTexture
{
};

#ifdef XRRENDERENGINEGL_EXPORTS
XRRenderExport XRTexture* xrCreateTexture(XRTextureData const* loadable);
#else
extern XRTexture* (*xrCreateTexture)(XRTextureData const* loadable);
#endif