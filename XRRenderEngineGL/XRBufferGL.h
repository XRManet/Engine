#pragma once

#include <XRFrameworkBase/XRBuffer.h>

class XRBufferGL : public XRBufferRHI
{
private:
	GLuint _buffer;

public:
	XRBufferGL() {}
	virtual ~XRBufferGL() {};

public:
	void Initialize();
};