#pragma once

#include "stdafx.h"

struct XRBufferCreateInfo;
class XRBuffer;
class XRBufferRHI;

#ifdef XRRENDERENGINEGL_EXPORTS
XRRenderExport XRBuffer* xrCreateBuffer(XRBufferCreateInfo const* createInfo);
#else
extern XRBuffer* (*xrCreateBuffer)(XRBufferCreateInfo const* createInfo);
#endif

struct XRBufferCreateInfo
{
};

class XRBufferRHI
{
public:
	virtual ~XRBufferRHI() {}
};

class XRBaseExport XRBuffer
{
#ifdef XRRENDERENGINEGL_EXPORTS
	/** @brief	Factory function */
	friend XRBuffer* xrCreateBuffer(XRBufferCreateInfo const* createInfo);
#endif

protected:
	XRBufferRHI* _rhi;

public:
	XRBuffer(XRBufferCreateInfo const* bufferCreateInfo) {}
	virtual ~XRBuffer() {}
};