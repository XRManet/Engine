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

	GLuint attributeIndex = 0;
	if (true)
	{
		GL_CALL(glBindVertexArray(_vao));
		auto& inputLayoutDesc = getInputLayoutDesc();
		for (uint32_t i = 0; i < inputLayoutDesc.getNumVertexBuffers(); ++i)
		{
			auto& bufferDesc = static_cast<XRVertexBufferDescGL const&>(inputLayoutDesc.getVertexBufferDesc(i));
			uint32_t numAttributes = static_cast<int>(bufferDesc.attributes.size());
			for (uint32_t j = 0; j < numAttributes; ++j)
			{
				auto& attributeDesc = static_cast<XRVertexAttributeDescGL const&>(bufferDesc.attributes[j]);
				XRFormatGL const& formatGL = static_cast<XRFormatGL const&>(attributeDesc.format);
				XRFormatGL::VertexAttribute vertexAttribute = formatGL.getGLVertexAttribute();
				GL_CALL(glEnableVertexAttribArray(attributeIndex));
				GL_CALL(glVertexAttribPointer(attributeIndex, vertexAttribute._numComponents, vertexAttribute._type, GL_FALSE, bufferDesc.stride, reinterpret_cast<void*>(attributeDesc.offset)));

				++attributeIndex;
			}
		}
		GL_CALL(glBindVertexArray(0));
	}
	else
	{
		// bind 필요 없는 객체 함수
		XRFormatGL::VertexAttribute vertexAttribute {};
		glVertexArrayAttribFormat(_vao, attributeIndex, vertexAttribute._numComponents, vertexAttribute._type, GL_FALSE, 0);
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
	glBindVertexArray(_vao);
	
	constexpr GLuint vertexBufferCount = 3;
	constexpr GLuint startBindingIndex = 0;
	GLuint vertexBufferNames[vertexBufferCount] = { 0, 0, 0 };
	GLintptr vertexBufferStartOffsets[vertexBufferCount] = {0, 0, 0};
	GLsizei vertexBufferStrides[vertexBufferCount] = {0, 0, 0};
	//uint vaobj, uint first, sizei count,const uint* buffers,const intptr* offsets,const sizei*strides
	glVertexArrayVertexBuffers(_vao, startBindingIndex, vertexBufferCount, vertexBufferNames, vertexBufferStartOffsets, vertexBufferStrides);
	
	GLuint elementBufferName = -1;
	glVertexArrayElementBuffer(_vao, elementBufferName);
}

void* XRInputLayoutGL::generateVertexBuffers() const
{
	return nullptr;
}

static XRInputLayoutGL* GetInputLayoutByKey(uint32_t keyInputLayout)
{
    return nullptr;
}

static bool InsertInputLayout(uint32_t keyInputLayout, XRInputLayoutGL* inputLayoutGL)
{
    return false;
}

XRModelGL::XRModelGL(XRModelData const* data) : XRModel(data)
{
	const auto* header = _data->GetHeader();
    assert(header->_numMeshes > 0);
    
    XRInputLayoutGL* inputLayout = nullptr;
    if (header->_keyInputLayout != 0)
	{
        inputLayout = GetInputLayoutByKey(header->_keyInputLayout);
    }
    else
    {
        inputLayout = new XRInputLayoutGL(header);
        
        InsertInputLayout(0, inputLayout);
    }
    
	_inputLayout = inputLayout;

	_meshes.reserve(header->_numMeshes);
	for (uint32_t m = 0; m < header->_numMeshes; ++m)
	{
		uint32_t numVertexBuffers = _inputLayout->getNumVertexBuffers();
		uint32_t numIndexBuffer = (header->_meshes[m]->_submeshes[0]->IsIndexed() ? 1 : 0);
		GLBuffer buffer {};
		buffer._numBuffers = numVertexBuffers + numIndexBuffer;
		GL_CALL(glGenBuffers(buffer._numBuffers, buffer._vbo));

		_meshes.emplace_back(buffer);

		for (uint32_t i = 0; i < numVertexBuffers; ++i)
		{
			GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _meshes[m]._vbo[i]));
			GL_CALL(glBufferData(GL_ARRAY_BUFFER, header->_meshes[m]->_submeshes[0]->getVertexBufferSize(i), nullptr, GL_STATIC_DRAW));

			GLuint offset = 0;
			GLuint size = header->_meshes[m]->_submeshes[0]->getVertexBufferSize(i);
			GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, header->_meshes[m]->_submeshes[0]->getVertexBuffer(i)));
		}

		if (numIndexBuffer == 1)
		{
			const uint32_t indexBufferId = _meshes[m]._numBuffers - 1;
			GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshes[m]._vbo[indexBufferId]));

			GLuint size = header->_meshes[m]->_submeshes[0]->getIndexBufferSize();
			GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, header->_meshes[m]->_submeshes[0]->getIndexBuffer(), GL_STATIC_DRAW));
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
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t XRModelGL::getNumVertices() const { return 0;
	//_data->GetHeader()->vertex_count;
	
}
uint32_t XRModelGL::getNumIndices() const { return 0;
	//_data->GetHeader()->index_count;
}
