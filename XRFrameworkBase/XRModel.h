#pragma once

#include "stdafx.h"

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

struct XRFormat
{
	enum {
		UNKNOWN,
		R4G4_UNORM_PACK8,
		R4G4B4A4_UNORM_PACK16,
		B4G4R4A4_UNORM_PACK16,
		R5G6B5_UNORM_PACK16,
		B5G6R5_UNORM_PACK16,
		R5G5B5A1_UNORM_PACK16,
		B5G5R5A1_UNORM_PACK16,
		A1R5G5B5_UNORM_PACK16,
		R8_UNORM,
		R8_SNORM0,
		R8_USCALED,
		R8_SSCALED,
		R8_UINT,
		R8_SINT,
		R8_SRGB,
		R8G8_UNORM,
		R8G8_SNORM,
		R8G8_USCALED,
		R8G8_SSCALED,
		R8G8_UINT,
		R8G8_SINT,
		R8G8_SRGB,
		R8G8B8_UNORM,
		R8G8B8_SNORM,
		R8G8B8_USCALED,
		R8G8B8_SSCALED,
		R8G8B8_UINT,
		R8G8B8_SINT,
		R8G8B8_SRGB,
		B8G8R8_UNORM,
		B8G8R8_SNORM,
		B8G8R8_USCALED,
		B8G8R8_SSCALED,
		B8G8R8_UINT,
		B8G8R8_SINT,
		B8G8R8_SRGB,
		R8G8B8A8_TYPELESS,
		R8G8B8A8_UNORM,
		R8G8B8A8_SNORM,
		R8G8B8A8_USCALED,
		R8G8B8A8_SSCALED,
		R8G8B8A8_UINT,
		R8G8B8A8_SINT,
		R8G8B8A8_SRGB,
		B8G8R8A8_UNORM,
		B8G8R8A8_SNORM,
		B8G8R8A8_USCALED,
		B8G8R8A8_SSCALED,
		B8G8R8A8_UINT,
		B8G8R8A8_SINT,
		B8G8R8A8_SRGB,
		A8B8G8R8_UNORM_PACK32,
		A8B8G8R8_SNORM_PACK32,
		A8B8G8R8_USCALED_PACK32,
		A8B8G8R8_SSCALED_PACK32,
		A8B8G8R8_UINT_PACK32,
		A8B8G8R8_SINT_PACK32,
		A8B8G8R8_SRGB_PACK32,
		A2R10G10B10_UNORM_PACK32,
		A2R10G10B10_SNORM_PACK32,
		A2R10G10B10_USCALED_PACK32,
		A2R10G10B10_SSCALED_PACK32,
		A2R10G10B10_UINT_PACK32,
		A2R10G10B10_SINT_PACK32,
		A2B10G10R10_UNORM_PACK32,
		A2B10G10R10_SNORM_PACK32,
		A2B10G10R10_USCALED_PACK32,
		A2B10G10R10_SSCALED_PACK32,
		A2B10G10R10_UINT_PACK32,
		A2B10G10R10_SINT_PACK32,
		R16_UNORM,
		R16_SNORM,
		R16_USCALED,
		R16_SSCALED,
		R16_UINT,
		R16_SINT,
		R16_SFLOAT,
		R16G16_UNORM,
		R16G16_SNORM,
		R16G16_USCALED,
		R16G16_SSCALED,
		R16G16_UINT,
		R16G16_SINT,
		R16G16_SFLOAT,
		R16G16B16_UNORM,
		R16G16B16_SNORM,
		R16G16B16_USCALED,
		R16G16B16_SSCALED,
		R16G16B16_UINT,
		R16G16B16_SINT,
		R16G16B16_SFLOAT,
		R16G16B16A16_UNORM,
		R16G16B16A16_SNORM,
		R16G16B16A16_USCALED, // VkFormat Only
		R16G16B16A16_SSCALED, // VkFormat Only
		R16G16B16A16_UINT,
		R16G16B16A16_SINT,
		R16G16B16A16_SFLOAT,
		R32_UINT,
		R32_SINT,
		R32_SFLOAT,
		R32G32_UINT,
		R32G32_SINT,
		R32G32_SFLOAT,
		R32G32B32_UINT,
		R32G32B32_SINT,
		R32G32B32_SFLOAT,
		R32G32B32A32_UINT,
		R32G32B32A32_SINT,
		R32G32B32A32_SFLOAT,
		R64_UINT,
		R64_SINT,
		R64_SFLOAT,
		R64G64_UINT,
		R64G64_SINT,
		R64G64_SFLOAT,
		R64G64B64_UINT,
		R64G64B64_SINT,
		R64G64B64_SFLOAT,
		R64G64B64A64_UINT,
		R64G64B64A64_SINT,
		R64G64B64A64_SFLOAT,
		B10G11R11_UFLOAT_PACK32, // VkFormat Only
		E5B9G9R9_UFLOAT_PACK32,
		D16_UNORM,
		X8_D24_UNORM_PACK32,
		D32_SFLOAT,
		S8_UINT,
		D16_UNORM_S8_UINT,
		D24_UNORM_S8_UINT,
		D32_SFLOAT_S8_UINT,
		D32_SFLOAT_S8X24_UINT, // DXGI_FORMAT Only
		R32_SFLOAT_X8X24_TYPELESS, // DXGI_FORMAT Only
		X32_TYPELESS_G8X24_UINT, // DXGI_FORMAT Only
		BC1_RGB_UNORM_BLOCK,
		BC1_RGB_SRGB_BLOCK,
		BC1_RGBA_UNORM_BLOCK,
		BC1_RGBA_SRGB_BLOCK,
		BC2_UNORM_BLOCK,
		BC2_SRGB_BLOCK,
		BC3_UNORM_BLOCK,
		BC3_SRGB_BLOCK,
		BC4_UNORM_BLOCK,
		BC4_SNORM_BLOCK,
		BC5_UNORM_BLOCK,
		BC5_SNORM_BLOCK,
		BC6H_UFLOAT_BLOCK,
		BC6H_SFLOAT_BLOCK,
		BC7_UNORM_BLOCK,
		BC7_SRGB_BLOCK,
		ETC2_R8G8B8_UNORM_BLOCK,
		ETC2_R8G8B8_SRGB_BLOCK,
		ETC2_R8G8B8A1_UNORM_BLOCK,
		ETC2_R8G8B8A1_SRGB_BLOCK,
		ETC2_R8G8B8A8_UNORM_BLOCK,
		ETC2_R8G8B8A8_SRGB_BLOCK,
		EAC_R11_UNORM_BLOCK,
		EAC_R11_SNORM_BLOCK,
		EAC_R11G11_UNORM_BLOCK,
		EAC_R11G11_SNORM_BLOCK,
		ASTC_4x4_UNORM_BLOCK,
		ASTC_4x4_SRGB_BLOCK,
		ASTC_5x4_UNORM_BLOCK,
		ASTC_5x4_SRGB_BLOCK,
		ASTC_5x5_UNORM_BLOCK,
		ASTC_5x5_SRGB_BLOCK,
		ASTC_6x5_UNORM_BLOCK,
		ASTC_6x5_SRGB_BLOCK,
		ASTC_6x6_UNORM_BLOCK,
		ASTC_6x6_SRGB_BLOCK,
		ASTC_8x5_UNORM_BLOCK,
		ASTC_8x5_SRGB_BLOCK,
		ASTC_8x6_UNORM_BLOCK,
		ASTC_8x6_SRGB_BLOCK,
		ASTC_8x8_UNORM_BLOCK,
		ASTC_8x8_SRGB_BLOCK,
		ASTC_10x5_UNORM_BLOCK,
		ASTC_10x5_SRGB_BLOCK,
		ASTC_10x6_UNORM_BLOCK,
		ASTC_10x6_SRGB_BLOCK,
		ASTC_10x8_UNORM_BLOCK,
		ASTC_10x8_SRGB_BLOCK,
		ASTC_10x10_UNORM_BLOCK,
		ASTC_10x10_SRGB_BLOCK,
		ASTC_12x10_UNORM_BLOCK,
		ASTC_12x10_SRGB_BLOCK,
		ASTC_12x12_UNORM_BLOCK,
		ASTC_12x12_SRGB_BLOCK,
		G8B8G8R8_422_UNORM,
		B8G8R8G8_422_UNORM,
		G8_B8_R8_3PLANE_420_UNORM,
		G8_B8R8_2PLANE_420_UNORM,
		G8_B8_R8_3PLANE_422_UNORM,
		G8_B8R8_2PLANE_422_UNORM,
		G8_B8_R8_3PLANE_444_UNORM,
		R10X6_UNORM_PACK16,
		R10X6G10X6_UNORM_2PACK16,
		R10X6G10X6B10X6A10X6_UNORM_4PACK16,
		G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
		B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
		G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
		G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
		G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
		G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
		G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
		R12X4_UNORM_PACK16,
		R12X4G12X4_UNORM_2PACK16,
		R12X4G12X4B12X4A12X4_UNORM_4PACK16,
		G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
		B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
		G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
		G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
		G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
		G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
		G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
		G16B16G16R16_422_UNORM,
		B16G16R16G16_422_UNORM,
		G16_B16_R16_3PLANE_420_UNORM,
		G16_B16R16_2PLANE_420_UNORM,
		G16_B16_R16_3PLANE_422_UNORM,
		G16_B16R16_2PLANE_422_UNORM,
		G16_B16_R16_3PLANE_444_UNORM,
		PVRTC1_2BPP_UNORM_BLOCK_IMG,
		PVRTC1_4BPP_UNORM_BLOCK_IMG,
		PVRTC2_2BPP_UNORM_BLOCK_IMG,
		PVRTC2_4BPP_UNORM_BLOCK_IMG,
		PVRTC1_2BPP_SRGB_BLOCK_IMG,
		PVRTC1_4BPP_SRGB_BLOCK_IMG,
		PVRTC2_2BPP_SRGB_BLOCK_IMG,
		PVRTC2_4BPP_SRGB_BLOCK_IMG,
		ASTC_4x4_SFLOAT_BLOCK_EXT,
		ASTC_5x4_SFLOAT_BLOCK_EXT,
		ASTC_5x5_SFLOAT_BLOCK_EXT,
		ASTC_6x5_SFLOAT_BLOCK_EXT,
		ASTC_6x6_SFLOAT_BLOCK_EXT,
		ASTC_8x5_SFLOAT_BLOCK_EXT,
		ASTC_8x6_SFLOAT_BLOCK_EXT,
		ASTC_8x8_SFLOAT_BLOCK_EXT,
		ASTC_10x5_SFLOAT_BLOCK_EXT,
		ASTC_10x6_SFLOAT_BLOCK_EXT,
		ASTC_10x8_SFLOAT_BLOCK_EXT,
		ASTC_10x10_SFLOAT_BLOCK_EXT,
		ASTC_12x10_SFLOAT_BLOCK_EXT,
		ASTC_12x12_SFLOAT_BLOCK_EXT,
		// DXGI_FORMAT Only, Query for size of memory
		R16_TYPELESS,
		R16G16B16A16_TYPELESS,
		R32G32_TYPELESS,
		R32G32B32_TYPELESS,
		R32G32B32A32_TYPELESS,
		R32G8X24_TYPELESS,
		R10G10B10A2_TYPELESS,
		MAX_COUNT,
		// DXGI_FORMAT Only, but reverse-ordered VkFormat exists
		R24_UNORM_X8_TYPELESS = X8_D24_UNORM_PACK32, // R-D mismatch
		R10G10B10A2_UNORM = A2B10G10R10_UNORM_PACK32,
		R10G10B10A2_UINT = A2B10G10R10_UINT_PACK32,
		R11G11B10_FLOAT = B10G11R11_UFLOAT_PACK32,
	};

private:
	uint32_t _value = XRFormat::UNKNOWN;
	
public:
	XRFormat() = default;
	XRFormat(uint32_t rhs) : _value(rhs) {}
	inline uint32_t operator = (uint32_t rhs) { return _value = rhs; }

protected:
	inline uint32_t getValue() const { return _value; }
	
public:
	uint32_t getCommonSize() const;
};

struct XRFormatInfo
{
	uint32_t channelsPerTexel;
	uint32_t bytesizePerBlock;
	uint32_t texelsPerBlock;
};

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
	XRInputLayoutDesc(std::vector<XRVertexBufferDesc>&& vertexBuffers);
	XRInputLayoutDesc(const XRObjectHeader* objectHeader) {}
	
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

	virtual uint32_t getNumVertices(uint32_t meshIndex, uint32_t submeshIndex) const { return 0; }
	virtual uint32_t getNumIndices(uint32_t meshIndex, uint32_t submeshIndex) const { return 0; }
};

#ifdef XRRENDERENGINEGL_EXPORTS
XRRenderExport XRInputLayout* xrCreateInputLayout(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize);
XRRenderExport XRModel* xrCreateModel(XRModelData const* loadable);
#else
extern XRModel* (*xrCreateModel)(XRModelData const* loadable);
extern XRInputLayout* (*xrCreateInputLayout)(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize);
#endif
