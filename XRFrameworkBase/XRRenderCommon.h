#pragma once

#include <stdint.h>

#include "XRFrameworkBase/XRPrimitiveTypes.h"
#include "XRFrameworkBase/XRUtility.h"

struct XRType
{
	enum : uint32_t {
		Sint,
		Uint,
		Float,
	} _value;
};

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

	operator uint32_t const& () const { return _value; }
	operator uint32_t& () { return _value; }

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

enum class XRResourceType
{
	Buffer,
	Texture1D,
	Texture1DArray,
	Texture2D,
	Texture2DArray,
	Texture2DMS,
	Texture2DMSArray,
	TextureCube,
	TextureCubeArray,
	Texture3D,
};

struct XRResourceUsageBits
{
	STATIC_ENUM_BIT_BEGIN;
	ADD_BIT_ENUM(CopySource, _copySource);
	ADD_BIT_ENUM(CopyDestination, _copyDestination);
	ADD_BIT_ENUM(Sampled, _sampled);
	ADD_BIT_ENUM(Fetched, _fetched);
	ADD_BIT_ENUM(RenderTarget, _renderTarget);
	ADD_BIT_ENUM(DepthStencil, _depthStencil);
	ADD_BIT_ENUM(Transient, _transientAttachable);
	ADD_BIT_ENUM(Input, _input);

	operator uint32_t const& () const { return reinterpret_cast<uint32_t const&>(*this); }
	operator uint32_t& () { return reinterpret_cast<uint32_t&>(*this); }
};

struct XRPrimitiveTopology
{
	enum {
		UNKNOWN,
		PointList,
		LineList,
		LineListWithAdjacency,
		LineStrip,
		LineStripWithAdjacency,
		TriangleList,
		TriangleListWithAdjacency,
		TriangleFan,
		TriangleStrip,
		TriangleStripWithAdjacency,
		QuadList,
		PatchList,
	};

	//static constexpr XRPrimitiveTopology PointList = Value::PointList;
	//static constexpr XRPrimitiveTopology LineList = Value::LineList;
	//static constexpr XRPrimitiveTopology LineListWithAdjacency = Value::LineListWithAdjacency;
	//static constexpr XRPrimitiveTopology LineStrip = Value::LineStrip;
	//static constexpr XRPrimitiveTopology LineStripWithAdjacency = Value::LineStripWithAdjacency;
	//static constexpr XRPrimitiveTopology TriangleList = Value::TriangleList;
	//static constexpr XRPrimitiveTopology TriangleListWithAdjacency = Value::TriangleListWithAdjacency;
	//static constexpr XRPrimitiveTopology TriangleFan = Value::TriangleFan;
	//static constexpr XRPrimitiveTopology TriangleStrip = Value::TriangleStrip;
	//static constexpr XRPrimitiveTopology TriangleStripWithAdjacency = Value::TriangleStripWithAdjacency;
	//static constexpr XRPrimitiveTopology QuadList = Value::QuadList;
	//static constexpr XRPrimitiveTopology PatchList = Value::PatchList;

private:
	uint32_t _value = UNKNOWN;

public:
	XRPrimitiveTopology() = default;
	inline XRPrimitiveTopology(uint32_t rhs) : _value(rhs) {}
	//inline uint32_t operator = (uint32_t rhs) { return _value = static_cast<Value>(rhs); }
	//inline operator uint32_t() const 	{ return _value; }

	inline operator uint32_t const& () const { return reinterpret_cast<uint32_t const&>(_value); }
	inline operator uint32_t& () { return reinterpret_cast<uint32_t&>(_value); }

public:
	inline uint32_t getNumVertices() const
	{
		switch (_value)
		{
		case UNKNOWN:					return 0;
		case PointList:					return 1;
		case LineList:					return 2;
		case LineListWithAdjacency:		return 2;
		case LineStrip:					return 2;
		case LineStripWithAdjacency:	return 2;
		case TriangleList:				return 3;
		case TriangleListWithAdjacency:	return 3;
		case TriangleFan:				return 3;
		case TriangleStrip:				return 3;
		case TriangleStripWithAdjacency:return 3;
		case QuadList:					return 4;
		case PatchList:					return 0;
		}
		return 0;
	}
};

struct XRIndexType
{
	enum {
		Index8,
		Index16,
		Index32,
	};

private:
	uint32_t _value = Index32;

public:
	XRIndexType() = default;
	XRIndexType(uint32_t rhs) : _value(rhs) {}
	inline uint32_t operator = (uint32_t rhs) { return _value = rhs; }
	inline operator uint32_t() const { return _value; }

public:
	inline uint32_t getIndexSize() const
	{
		switch (_value)
		{
		case Index8:					return 1;
		case Index16:					return 2;
		case Index32:					return 4;
		}
		return 0;
	}

};

/**
 * @union	XRClearValue
 *
 * @brief	MSVS 2019 빌드 기준, 16.1 version 이상의 std:c++latest 에서 빌드해야 함.
 *
 * @author	Jiman Jeong
 * @date	2021-02-15
 */

union XRClearValue
{
	union {
		xr::vec4<float_t> _float;
		xr::vec4<int32_t> _int;
		xr::vec4<uint32_t> _uint;
	} _color;

	struct {
		float_t _depth;
		uint32_t _stencil;
	};

public:
	XRClearValue() : _color{ ._float = {} } {}
	XRClearValue(XRClearValue const& rhs) = default;

	XRClearValue(xr::vec4<float_t> const& floatClear) : _color{ ._float = floatClear } {}
	XRClearValue(xr::vec4<int32_t> const& intClear) : _color{ ._int = intClear } {}
	XRClearValue(xr::vec4<uint32_t> const& uintClear) : _color{ ._uint = uintClear } {}
	
	XRClearValue(float_t depthClear, uint32_t stencilClear) : _depth(depthClear), _stencil(stencilClear) {}
};

struct XRBarrierDescription
{

};
