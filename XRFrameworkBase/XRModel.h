#pragma once

#include "stdafx.h"
#include "XRRenderCommon.h"

struct XRMaterialParameter
{
	
};

class XRMaterial
{
	using NameKey_t = uint32_t;
	using OrderValue_t = uint32_t;
	std::unordered_map<NameKey_t, OrderValue_t> _nameOrderMap;
	std::vector<XRMaterialParameter> _materialParameters;
};

class XRModelData;
// #include "ModelDataRepresentation.h"

class XRModel;

class XRInputLayoutDesc;
struct XRVertexAttributeDesc
{
	friend XRInputLayoutDesc;
	
protected:
	uint16_t bindingIndex = -1;
	uint16_t subIndex = -1; // Note(jiman): 이거 뭐에다 쓰는거지
	uint32_t shaderLocation = -1;
	uint32_t offset = -1;
public:
	XRFormat format = XRFormat::UNKNOWN;
};

struct XRBaseExport XRVertexBufferDesc
{
	friend XRInputLayoutDesc;
	
protected:
	uint32_t bindingIndex = -1;
	uint32_t stride = 0;
public:
	// if instanceDivisor == 0, then it is per vertex.
	// if instanceDivisor > 0, then it is per instance and divided by instanceDivisor.
	// if instanceDivisor == -1, divisor feature is not supported.
	uint32_t instanceDivisor = 0;

	std::vector<XRVertexAttributeDesc> attributes;
};

enum class PreferredStrideSizeOption : uint32_t
{
	DenyGroup = 0,
	TotalSizeInAttributes = uint32_t(-1),

	Default = DenyGroup,
};

struct XRSubmeshHeader
{
	uint32_t _defaultInputLayoutKey = 0;
	uint32_t _numMaterials = 0;

	uint32_t _offsetMaterialKeys = 0;
	uint32_t _offsetIndex = 0;
	uint32_t _offsetVertexOffsets[0];
    
    bool IsIndexed() const { return (_offsetIndex != 0); }
	const uint32_t* getMaterialKeys() const
	{
		return reinterpret_cast<const uint32_t*>(reinterpret_cast<const uint8_t*>(this) + _offsetMaterialKeys);
	}
	uint32_t* getMaterialKeys()
	{
		return const_cast<uint32_t*>(const_cast<XRSubmeshHeader const*>(this)->getMaterialKeys());
	}
	const uint8_t* getVertexBuffer(uint32_t slotIndex) const
	{
		return reinterpret_cast<const uint8_t*>(this) + _offsetVertexOffsets[slotIndex];
	}
	uint8_t* getVertexBuffer(uint32_t slotIndex)
	{
		return const_cast<uint8_t*>(const_cast<XRSubmeshHeader const*>(this)->getVertexBuffer(slotIndex));
	}
	const uint32_t* getIndexBuffer() const
	{
		return reinterpret_cast<const uint32_t*>(reinterpret_cast<const uint8_t*>(this) + _offsetIndex);
	}
	uint32_t* getIndexBuffer()
	{
		return const_cast<uint32_t*>(const_cast<XRSubmeshHeader const*>(this)->getIndexBuffer());
	}

	uint32_t getVertexBufferSize(uint32_t slotIndex) const
	{
		return _offsetVertexOffsets[slotIndex + 1] - _offsetVertexOffsets[slotIndex];
	}
	uint32_t getIndexBufferSize() const
	{
		return _offsetVertexOffsets[0] - _offsetIndex;
	}
};

struct XRMeshHeader
{
	XRPrimitiveTopology _topology;
	uint32_t _indexSize;
    
    uint32_t _numSubmeshes;
    XRSubmeshHeader* _submeshes[0];
};

constexpr size_t meshHeaderSize = sizeof(XRMeshHeader);
static_assert(meshHeaderSize == 16, "TEST");

#define XR_FOURCC(fcc) *((const uint32_t*)#fcc)
struct XRObjectHeader
{
    uint32_t _fccIdentifier = XR_FOURCC(XRP1);
	// if _defaultInputLayoutKey == 0, then header has no known layout.
	// else if _defaultInputLayoutKey == -1, then each submeshes has its own layout.
	// otherwise, then there is one unified layout.
    uint32_t _defaultInputLayoutKey = 0;
    
    uint32_t _numMeshes;
    XRMeshHeader* _meshes[0];
};
constexpr size_t objectHeaderSize = sizeof(XRObjectHeader);

class XRBaseExport XRInputLayoutDesc
{
private:
	std::vector<XRVertexBufferDesc> _vertexBuffers;
	uint32_t _hash = 0;
	
public:
	XRInputLayoutDesc(std::vector<XRVertexBufferDesc>& vertexBuffers);
	XRInputLayoutDesc(const XRObjectHeader* objectHeader) {}

public:
	void append(XRInputLayoutDesc& inputLayoutDesc);
	void append(std::vector<XRVertexBufferDesc>& vertexBufferDescs);
	
public:
	inline uint32_t getStride(uint32_t bufferIndex) const { return _vertexBuffers[bufferIndex].stride; }
	inline uint32_t getNumVertexBuffers() const { return static_cast<uint32_t>(_vertexBuffers.size()); }
	inline const XRVertexBufferDesc& getVertexBufferDesc(uint32_t bufferIndex) const { return _vertexBuffers[bufferIndex]; }
	
private:
	uint32_t calcHash();
	
public:
	uint32_t getHash()
	{
		if (_hash != 0) return _hash;
		return calcHash();
	}
};

class XRBaseExport XRInputLayout
{
private:
	XRInputLayoutDesc _inputLayoutDesc;
	uint32_t _preferredStride;

public:
	XRInputLayout(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize)
		: _preferredStride(preferredStrideSize), _inputLayoutDesc(std::move(inputLayoutDesc)) {}
	XRInputLayout(const XRObjectHeader* objectHeader)
		: _inputLayoutDesc(objectHeader) {}
	virtual ~XRInputLayout() {}

public:
	virtual void bind() const {}
	
public:
	inline uint32_t getStride(uint32_t bufferIndex) const { return getInputLayoutDesc().getStride(bufferIndex); }
	inline uint32_t getNumVertexBuffers() const { return getInputLayoutDesc().getNumVertexBuffers(); }

protected:
	XRInputLayoutDesc const& getInputLayoutDesc() const { return _inputLayoutDesc; }

public:
	static XRInputLayout* GetInputLayoutByKey(uint32_t keyInputLayout);
	static bool InsertInputLayout(uint32_t keyInputLayout, XRInputLayout* inputLayout);
	static uint32_t getNumInputLayouts();
};

class XRBaseExport XRModel
{
protected:
	union {
		XRModelData const* _data;
		// Todo) Other types of data for its own rendering engine would be given.
		// To support them, the XRModel has to bind data dynamically.
		// The XRModel introduces common interfaces of model data to a XRObject.
	};
	XRInputLayout* _inputLayout = nullptr;

public:
	XRModel(XRModelData const* data);
	virtual ~XRModel();

public:
	virtual void bind() const {}
	void bindWithInputLayout() const
	{
		if (_inputLayout != nullptr)
			_inputLayout->bind();

		bind();
	}

	virtual uint32_t getNumVertices() const { return 0; }
	virtual uint32_t getNumVertices(uint32_t meshIndex) const { return 0; }
	virtual uint32_t getNumVertices(uint32_t meshIndex, uint32_t submeshIndex) const { return 0; }
	virtual uint32_t getNumIndices() const { return 0; }
	virtual uint32_t getNumIndices(uint32_t meshIndex) const { return 0; }
	virtual uint32_t getNumIndices(uint32_t meshIndex, uint32_t submeshIndex) const { return 0; }

public:
	XRInputLayout const* getInputLayout() const { return _inputLayout; }
};

#ifdef XRRHIOPENGL_EXPORTS
XRRenderExport XRInputLayout* xrCreateInputLayout(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize);
XRRenderExport XRModel* xrCreateModel(XRModelData const* loadable);
#else
extern XRModel* (*xrCreateModel)(XRModelData const* loadable);
extern XRInputLayout* (*xrCreateInputLayout)(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize);
#endif
