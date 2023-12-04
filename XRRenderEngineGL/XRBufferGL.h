#pragma once

#include <XRFrameworkBase/XRBuffer.h>

class XRBufferGL : public XRBufferRHI
{
public:
	XRBufferGL(XRRenderDevice* ownerRenderDevice) : XRBufferRHI(ownerRenderDevice), _target(GL_NONE), _buffer(GL_NONE) {}
	virtual ~XRBufferGL() {};

public:
	inline GLuint getBufferId() const { return _buffer; }

public:
	void Initialize(XRBuffer* bufferHandle);

private:
	GLenum _target;
	GLuint _buffer;
};