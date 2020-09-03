#pragma once

#include <XRFrameworkBase/XRModel.h>

class XRInputLayoutGL : public XRInputLayout
{
private:
	unsigned _vao;

public:
	XRInputLayoutGL(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize);
	XRInputLayoutGL(const XRObjectHeader* header);

	virtual ~XRInputLayoutGL();

public:
	void bind() const override;
	void bindAttributes(uint32_t bindingIndex) const;
	void* generateVertexBuffers() const;
};

constexpr uint32_t MAX_NUM_SLOTS = 4;
class XRModelGL : public XRModel
{
private:
	struct GLBuffer {
		GLuint _vbo[MAX_NUM_SLOTS];
		uint32_t _numBuffers;
	};

	std::vector<GLBuffer> _meshes;
	mutable bool _isBoundWithVAO = false;

public:
	XRModelGL(XRModelData const* data);
	virtual ~XRModelGL();


public:
	void bind() const override;

	uint32_t getNumVertices() const override;
	uint32_t getNumIndices() const override;
};
