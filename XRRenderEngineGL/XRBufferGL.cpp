#include "stdafx.h"

#include "XRBufferGL.h"

extern GLint MAX_UNIFORM_BUFFER_BINDINGS;
extern GLint MAX_UNIFORM_LOCATIONS;
extern GLint UNIFORM_BUFFER_OFFSET_ALIGNMENT;
extern GLint MAX_UNIFORM_BLOCK_SIZE;

extern GLint MAX_VARYING_COMPONENTS;

extern GLint MAX_TEXTURE_IMAGE_UNITS;
extern GLint MAX_COMBINED_TEXTURE_IMAGE_UNITS;
extern GLint MAX_COMPUTE_TEXTURE_IMAGE_UNITS;


void XRBufferGL::Initialize(XRBuffer* bufferHandle)
{
	glGenBuffers(1, &_buffer);
	_handle = bufferHandle;

	GLsizeiptr bufferSize = 0;
	bufferSize = NEXT_ALIGN_2(bufferSize + _handle->getBufferCreateInfo()->_size, UNIFORM_BUFFER_OFFSET_ALIGNMENT);

	glBindBuffer(GL_UNIFORM_BUFFER, _buffer);
	glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);

#if defined(_DEBUG) || defined(DEBUG)
	// Note: Core profile needs allocation first, then binding each range to corresponding index as we wish.
	{
		GLint allocatedSize = 0;
		glGetBufferParameteriv(GL_UNIFORM_BUFFER, GL_BUFFER_SIZE, &allocatedSize);
		assert(allocatedSize == bufferSize);
	}
#endif
}