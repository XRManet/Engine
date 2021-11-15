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
class XRView
{
	using ViewType = typename ResourceType::View;

	ResourceType*	_resource;
	ViewType		_view;
	AccessMask		_lastAccessMask;

public:
	XRView(ResourceType* resource)
		: _resource(resource), _view()
	{
	}

	XRView(ResourceType* resource, ViewType* view)
	{
	}

public:
	inline ViewType const& getViewInfo() const { return _view; }
	inline ResourceType* getResource() { return _resource; }
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

public:
	struct View
	{
		uint32_t			_offset;
		uint32_t			_size;
	};

protected:
	XRBufferRHI*			_rhi;
	XRBufferCreateInfo		_bufferCreateInfo;

public:
	XRBuffer(XRBufferCreateInfo const* bufferCreateInfo, XRBufferRHI* bufferRHI)
		: _rhi(bufferRHI), _bufferCreateInfo(*bufferCreateInfo)
	{}
	virtual ~XRBuffer() {}

public:
	XRBufferCreateInfo const* getBufferCreateInfo() const { return &_bufferCreateInfo; }

};