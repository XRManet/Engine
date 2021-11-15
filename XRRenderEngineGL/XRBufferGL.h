#pragma once

#include <XRFrameworkBase/XRBuffer.h>

class XRBufferGL : public XRBufferRHI
{
public:
	XRBufferGL() {}
	virtual ~XRBufferGL() {};

public:
	GLuint getBufferId() const { return _buffer; }

public:
	void Initialize(XRBuffer* bufferHandle);

private:
	GLuint _buffer;
};