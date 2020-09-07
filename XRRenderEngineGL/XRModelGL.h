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

struct MultiDrawElementsInfo
{
	GLsizei* indexCounts;	// in num 
	GLintptr* indexOffsets;	// in bytes
	GLsizei drawCount;
};

struct MultiDrawElementsBaseVertexInfo : public MultiDrawElementsInfo
{
	GLsizei* baseVertexOffsets;
};

constexpr uint32_t MAX_NUM_SLOTS = 4;
class XRModelGL : public XRModel
{
private:
	struct GLBuffer {
		GLuint _vbo[MAX_NUM_SLOTS];
		uint32_t _numBuffers; // 1 ~ MAX_NUM_SLOTS
		uint32_t _infoDataBufferOffset;
	};

	mutable std::vector<uint8_t> _infoDataBuffer;
	std::vector<GLBuffer> _meshes;

	mutable bool _isBoundWithVAO = false;

public:
	XRModelGL(XRModelData const* data);
	virtual ~XRModelGL();


public:
	void bind() const override;

	uint32_t getNumVertices(uint32_t meshIndex, uint32_t submeshIndex) const override;
	uint32_t getNumIndices(uint32_t meshIndex, uint32_t submeshIndex) const override;

	GLenum getIndexType(uint32_t meshIndex) const;
	void getMultiDrawElementsInfo(MultiDrawElementsInfo& info, uint32_t meshIndex) const;
	void getMultiDrawElementsBaseVertexInfo(MultiDrawElementsBaseVertexInfo& info, uint32_t meshIndex) const;
};
