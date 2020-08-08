#include "stdafx.h"
#include "XRModelGL.h"

#include <XRFrameworkBase/XRModelData.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>

struct XRFormatGL : public XRFormat
{
public:
	struct VertexAttribute
	{
		GLuint _numComponents;
		GLenum _type;
	};

public:
	inline VertexAttribute getGLVertexAttribute() const
	{
		switch (getValue())
		{
		case R32G32B32_SFLOAT:
			return VertexAttribute { 3, GL_FLOAT };
		case R32G32_SFLOAT:
			return VertexAttribute { 2, GL_FLOAT };
		default:
			assert(false);
			return VertexAttribute{};
		}
	}
};

XRInputLayoutGL::XRInputLayoutGL(XRInputLayoutDesc&& in_inputLayoutDesc, uint32_t preferredStrideSize)
	: XRInputLayout(std::move(in_inputLayoutDesc), preferredStrideSize)
{
	// Parse given model and construct input layout from it.
	// if then, we can get a layout of model and just use it later.
	GL_CALL(glGenVertexArrays(1, &_vao));

	struct XRVertexAttributeDescGL : XRVertexAttributeDesc { friend XRInputLayoutGL; };
	struct XRVertexBufferDescGL : XRVertexBufferDesc { friend XRInputLayoutGL; };

	GLuint bindingIndex = 0;
	GLuint attributeIndex = 0;
	if (true)
	{
		GL_CALL(glBindVertexArray(_vao));
		auto& inputLayoutDesc = getInputLayoutDesc();
		for (uint32_t i = 0; i < inputLayoutDesc.getNumVertexBuffers(); ++i)
		{
			auto& bufferDesc = static_cast<XRVertexBufferDescGL const&>(inputLayoutDesc.getVertexBufferDesc(i));
			uint32_t numAttributes = static_cast<int>(bufferDesc.attributes.size());

			assert(bufferDesc.bindingIndex == bindingIndex);

			GL_CALL(glVertexBindingDivisor(bindingIndex, bufferDesc.instanceDivisor));
			for (uint32_t j = 0; j < numAttributes; ++j)
			{
				auto& attributeDesc = static_cast<XRVertexAttributeDescGL const&>(bufferDesc.attributes[j]);
				XRFormatGL const& formatGL = static_cast<XRFormatGL const&>(attributeDesc.format);
				XRFormatGL::VertexAttribute vertexAttribute = formatGL.getGLVertexAttribute();
				GL_CALL(glEnableVertexAttribArray(attributeIndex));
#if 1
				GL_CALL(glVertexAttribBinding(attributeIndex, bindingIndex));

				GL_CALL(glVertexAttribFormat(attributeIndex, vertexAttribute._numComponents, vertexAttribute._type, GL_FALSE, attributeDesc.offset));
#else
				/* VertexAttribPointer == {
					VertexAttrib*Format(index, size, type, fnormalized, g, 0);
					VertexAttribBinding(index, index);
					if (stride != 0) {
						effectiveStride = stride;
					} else {
						compute effectiveStride based on size and type;
					}
					VERTEX_ATTRIB_ARRAY_STRIDE[index] = stride;
					// This sets VERTEX_BINDING_STRIDE to effectiveStride
					VERTEX_ATTRIB_ARRAY_POINTER[index] = pointer;
					BindVertexBuffer(index, buffer bound to ARRAY_BUFFER,
					(char *)pointer - (char *)NULL, effectiveStride);
				}
				*/
				GL_CALL(glVertexAttribPointer(attributeIndex, vertexAttribute._numComponents, vertexAttribute._type, GL_FALSE, bufferDesc.stride, reinterpret_cast<void*>(attributeDesc.offset)));
#endif
				++attributeIndex;
			}
			++bindingIndex;
		}
		GL_CALL(glBindVertexArray(0));
	}
	else
	{
		// bind 필요 없는 객체 함수 형식
		auto& inputLayoutDesc = getInputLayoutDesc();
		for (uint32_t i = 0; i < inputLayoutDesc.getNumVertexBuffers(); ++i)
		{
			auto& bufferDesc = static_cast<XRVertexBufferDescGL const&>(inputLayoutDesc.getVertexBufferDesc(i));
			uint32_t numAttributes = static_cast<int>(bufferDesc.attributes.size());

			assert(bufferDesc.bindingIndex == bindingIndex);

			glVertexArrayBindingDivisor(_vao, bindingIndex, bufferDesc.instanceDivisor);
			for (uint32_t j = 0; j < numAttributes; ++j)
			{
				auto& attributeDesc = static_cast<XRVertexAttributeDescGL const&>(bufferDesc.attributes[j]);
				XRFormatGL const& formatGL = static_cast<XRFormatGL const&>(attributeDesc.format);
				XRFormatGL::VertexAttribute vertexAttribute = formatGL.getGLVertexAttribute();
				/*
				API 이름이 이렇게 생긴 게 맘에 안들지만,
				glEnableVertexAttribArray() 가 아님에 주의
				*/
				glEnableVertexArrayAttrib(_vao, attributeIndex);

				glVertexArrayAttribBinding(_vao, attributeIndex, bindingIndex);
				glVertexArrayAttribFormat(_vao, attributeIndex, vertexAttribute._numComponents, vertexAttribute._type, GL_FALSE, attributeDesc.offset);
				++attributeIndex;
			}
			++bindingIndex;
		}
	}
}

XRInputLayoutGL::XRInputLayoutGL(const XRObjectHeader* objectHeader)
: XRInputLayout(objectHeader)
{
	assert(objectHeader->_numMeshes > 0);
	//assert(objectHeader->_numVertexAttributes > 0);
	
	// Parse given model and construct input layout from it.
	// if then, we can get a layout of model and just use it later.
	GL_CALL(glGenVertexArrays(1, &_vao));
	GL_CALL(glBindVertexArray(_vao));
	
	struct XRVertexAttributeDescGL : XRVertexAttributeDesc { friend XRInputLayoutGL; };
	struct XRVertexBufferDescGL : XRVertexBufferDesc { friend XRInputLayoutGL; };

	for(uint32_t i = 0; i < objectHeader->_numMeshes; ++i)
	{
		const auto& meshes = objectHeader->_meshes[i];
		uint32_t numSubmeshes = meshes->_numSubmeshes;
		for(uint32_t j = 0; j < numSubmeshes; ++j)
		{
			meshes->_submeshes[j];
		}
	}
	
	generateVertexBuffers();
	
	objectHeader->_numMeshes;
}

XRInputLayoutGL::~XRInputLayoutGL()
{
	//
	// If a vertex array object that is currently bound is deleted, the binding for that object reverts to zero and the default vertex array becomes current.
	GL_CALL(glDeleteVertexArrays(1, &_vao));
}

void XRInputLayoutGL::bind() const
{
	GL_CALL(glBindVertexArray(_vao));
}

void* XRInputLayoutGL::generateVertexBuffers() const
{
	return nullptr;
}

XRModelGL::XRModelGL(XRModelData const* data) : XRModel(data)
{
	const auto* header = _data->GetHeader();
    assert(header->_numMeshes > 0);
    
    XRInputLayoutGL* inputLayout = nullptr;
    if (header->_defaultInputLayoutKey != 0)
	{
        inputLayout = static_cast<XRInputLayoutGL*>(XRInputLayout::GetInputLayoutByKey(header->_defaultInputLayoutKey));
    }
    else
    {
		assert(false);
        inputLayout = new XRInputLayoutGL(header);
        
		XRInputLayout::InsertInputLayout(0, inputLayout);
    }
    
	_inputLayout = inputLayout;

	_meshes.reserve(header->_numMeshes);
	for (uint32_t m = 0; m < header->_numMeshes; ++m)
	{
		uint32_t numVertexBuffers = _inputLayout->getNumVertexBuffers();
		uint32_t numIndexBuffer = (header->_meshes[m]->_submeshes[0]->IsIndexed() ? 1 : 0);
		GLBuffer buffer {};
		buffer._numBuffers = numVertexBuffers + numIndexBuffer;

		for (uint32_t s = 0; s < header->_meshes[m]->_numSubmeshes; ++s)
		{
			GL_CALL(glGenBuffers(buffer._numBuffers, buffer._vbo));

			for (uint32_t i = 0; i < numVertexBuffers; ++i)
			{
				GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer._vbo[i]));

				GLuint offset = 0;
				GLuint size = header->_meshes[m]->_submeshes[s]->getVertexBufferSize(i);
				GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW));
				GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, header->_meshes[m]->_submeshes[s]->getVertexBuffer(i)));
			}

			if (numIndexBuffer == 1)
			{
				const uint32_t indexBufferId = buffer._numBuffers - 1;
				GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer._vbo[indexBufferId]));

				GLuint offset = 0;
				GLuint size = header->_meshes[m]->_submeshes[s]->getIndexBufferSize();
				GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW));
				GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, header->_meshes[m]->_submeshes[s]->getIndexBuffer()));
			}

			_meshes.emplace_back(buffer);
		}
	}
#if 0
    {
#if XR_MODEL_DATA_LAYOUT == XR_MODEL_DATA_LAYOUT_SOA
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, GL.position));
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * header->vertex_count * 2,
                             nullptr, GL_STATIC_DRAW));
        
        assert(header->vertex_count > 0);
        GLuint offset = 0;
        GLuint size = sizeof(uint32_t) * header->vertex_count;
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->vertex_offset));
        
        //if (header->normal_offset > 0)
        {
            offset = size;
            size += sizeof(XRModelData::VertexNormal) * header->vertex_count;
            //GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->normal_offset));
        }
        
        //if (header->texture_offset > 0)
        {
            offset = size;
            size += sizeof(XRModelData::TextureCoordinate) * header->vertex_count;
            //GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->texture_offset));
        }
#else
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, GL.vertex));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * header->vertex_count * 2,
			nullptr, GL_STATIC_DRAW));

		assert(header->vertex_count > 0);
		GLuint offset = 0;
		GLuint size = sizeof(uint32_t) * header->vertex_count;
		GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->vertex_offset));

		//if (header->normal_offset > 0)
		{
			offset = size;
			size += sizeof(XRModelData::VertexNormal) * header->vertex_count;
			//GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->normal_offset));
		}

		//if (header->texture_offset > 0)
		{
			offset = size;
			size += sizeof(XRModelData::TextureCoordinate) * header->vertex_count;
			//GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, address + header->texture_offset));
		}
#endif
	}
	if (header->IsIndexed())
	{
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL.index));

		GLuint size = sizeof(XRModelData::FaceIndexTriangle) * header->index_count;
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, address + header->index_offset, GL_STATIC_DRAW));
	}

#endif

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

XRModelGL::~XRModelGL()
{
	uint32_t numMeshes = _meshes.size();
	for (uint32_t m = 0; m < numMeshes; ++m)
	{
		GL_CALL(glDeleteBuffers(_meshes[m]._numBuffers, _meshes[m]._vbo));
	}
}

void XRModelGL::bind() const
{
#if XR_MODEL_DATA_LAYOUT == XR_MODEL_DATA_LAYOUT_SOA
    //glBindBuffer(GL_ARRAY_BUFFER, _meshes[0]._vbo[0]);
    //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    //
    //glBindBuffer(GL_ARRAY_BUFFER, GL.normal);
    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    //
    //glBindBuffer(GL_ARRAY_BUFFER, GL.textureCoord);
    //glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
#else
	glBindBuffer(GL_ARRAY_BUFFER, GL.vertex);
#endif
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL.index);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	static constexpr GLuint MAX_VERTEX_BUFFER_COUNT = 16;
	GLintptr vertexBufferStartOffsets[MAX_VERTEX_BUFFER_COUNT] = { 0, };
	GLsizei vertexBufferStrides[MAX_VERTEX_BUFFER_COUNT] = { 0, };

	const auto* header = _data->GetHeader();

	for (uint32_t m = 0; m < 1/*header->_numMeshes*/; ++m)
	{
		uint32_t numVertexBuffers = _inputLayout->getNumVertexBuffers();
		uint32_t numIndexBuffer = (header->_meshes[m]->_submeshes[0]->IsIndexed() ? 1 : 0);

		for (uint32_t s = 0; s < 1/*header->_meshes[m]->_numSubmeshes*/; ++s)
		{
			for (uint32_t i = 0; i < numVertexBuffers; ++i)
			{
				vertexBufferStartOffsets[i] = 0;
				vertexBufferStrides[i] = _inputLayout->getStride(i);
			}

			GLuint startBindingIndex = 0;
			GLuint const* vertexBufferNames = _meshes[m]._vbo;
#if 1
			GL_CALL(glBindVertexBuffers(startBindingIndex, numVertexBuffers, vertexBufferNames, vertexBufferStartOffsets, vertexBufferStrides));

			if (numIndexBuffer == 1)
			{
				GLuint elementBufferName = _meshes[m]._vbo[numVertexBuffers];
				GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferName));
			}
#else
			//uint vaobj, uint first, sizei count,const uint* buffers,const intptr* offsets,const sizei*strides
			glVertexArrayVertexBuffers(_vao, startBindingIndex, vertexBufferCount, vertexBufferNames, vertexBufferStartOffsets, vertexBufferStrides);

			GLuint elementBufferName = -1;
			glVertexArrayElementBuffer(_vao, elementBufferName);
#endif
		}
	}
}

uint32_t XRModelGL::getNumVertices() const { return 0;
	//_data->GetHeader()->vertex_count;
	
}
uint32_t XRModelGL::getNumIndices() const {
	return _data->GetHeader()->_meshes[0]->_submeshes[0]->getIndexBufferSize() / 2;
}
