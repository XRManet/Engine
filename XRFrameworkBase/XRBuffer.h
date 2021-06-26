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
	size_t _size;
};

using AccessMask = uint32_t;
template<typename ResourceType>
class XRResource
{
	using ViewType = typename XRResourceType::ViewType;

	ResourceType* _resource;
	ViewType* _view;
	AccessMask _lastAccessMask;

public:
	XRResource(ResourceType* resource)
		: _resource(resource), _view()
	{
	}

	XRResource(ResourceType* resource, ViewType* view)
	{
	}
};

class XRBufferRHI
{
protected:
	XRBuffer* _handle;

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
	XRBufferRHI*			_rhi;
	XRBufferCreateInfo		_bufferCreateInfo;

public:
	XRBuffer(XRBufferCreateInfo const* bufferCreateInfo)
		: _rhi(nullptr), _bufferCreateInfo(*bufferCreateInfo)
	{}
	virtual ~XRBuffer() {}

public:
	XRBufferCreateInfo const* GetBufferCreateInfo() const { return &_bufferCreateInfo; }
};