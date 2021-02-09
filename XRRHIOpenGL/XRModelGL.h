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
	GLsizei* _indexCounts;	// in num 
	GLintptr* _indexByteOffsets;	// in bytes
	GLsizei _drawCount;
};

struct MultiDrawElementsBaseVertexInfo : public MultiDrawElementsInfo
{
	GLsizei* _vertexBase; // in num
};

struct MultiDrawElementsIndirectInfo {
	struct DrawElementsIndirectCommand
	{
		GLuint  _indexCount;
		GLuint  _instanceCount;
		GLuint  _indexFirst;
		GLuint  _vertexBase;
		GLuint  _instanceBase;
	};

	DrawElementsIndirectCommand*	_indirectCommands;
	GLsizei							_drawCount;
	GLsizei							_stride;
};

constexpr uint32_t MAX_NUM_SLOTS = 4;
class XRModelGL : public XRModel
{
private:
	struct GLBuffer {
		GLuint _vbo[MAX_NUM_SLOTS];
		uint32_t _numBuffers; // 1 ~ MAX_NUM_SLOTS
	};

	struct MeshInfo
	{
		uint32_t _drawParameterBufferOffset;
	};

	GLBuffer						_meshes;
	std::vector<MeshInfo>			_meshInfos;
	mutable std::vector<uint8_t>	_drawParameterBuffer;

	mutable bool _isBoundWithVAO = false;

public:
	XRModelGL(XRModelData const* data);
	virtual ~XRModelGL();


public:
	void bind() const override;

	uint32_t getNumVertices() const override;
	uint32_t getNumVertices(uint32_t meshIndex) const override;
	uint32_t getNumVertices(uint32_t meshIndex, uint32_t submeshIndex) const override;
	uint32_t getNumIndices() const override;
	uint32_t getNumIndices(uint32_t meshIndex) const override;
	uint32_t getNumIndices(uint32_t meshIndex, uint32_t submeshIndex) const override;

	GLenum getIndexType(uint32_t meshIndex) const;
	void getMultiDrawElementsInfo(MultiDrawElementsInfo& info, uint32_t meshIndex) const;
	void getMultiDrawElementsBaseVertexInfo(MultiDrawElementsBaseVertexInfo& info, uint32_t meshIndex) const;
	void getMultiDrawElementsIndirectInfo(MultiDrawElementsIndirectInfo& info) const;
};
