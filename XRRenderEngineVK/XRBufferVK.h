#pragma once

#include <XRFrameworkBase/XRBuffer.h>

class XRBufferVK : public XRBufferRHI
{
public:
	XRBufferVK(XRRenderDevice* ownerRenderDevice) : XRBufferRHI(ownerRenderDevice), _buffer(VK_NULL_HANDLE) {}
	virtual ~XRBufferVK() {}

public:
	inline VkBuffer getVkBuffer() const { return _buffer; }

public:
	void Initialize(XRBuffer* bufferHandle);

private:
	VkBuffer _buffer;
};