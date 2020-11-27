#include "stdafx.h"

#include "XRRenderCommon.h"

uint32_t XRFormat::getCommonSize() const
{
	switch (_value)
	{
	case XRFormat::UNKNOWN:
		return 0;
	case XRFormat::R4G4_UNORM_PACK8:
		return 1;
	case XRFormat::R8G8_UNORM:
		return 2;
	case XRFormat::R8G8B8_UNORM:
		return 3;
	case XRFormat::R8G8B8A8_TYPELESS:
	case XRFormat::R8G8B8A8_UNORM:
	case XRFormat::R8G8B8A8_SNORM:
	case XRFormat::R8G8B8A8_USCALED:
	case XRFormat::R8G8B8A8_SSCALED:
	case XRFormat::R8G8B8A8_UINT:
	case XRFormat::R8G8B8A8_SINT:
	case XRFormat::R8G8B8A8_SRGB:
	case XRFormat::B8G8R8A8_UNORM:
	case XRFormat::B8G8R8A8_SNORM:
	case XRFormat::B8G8R8A8_USCALED:
	case XRFormat::B8G8R8A8_SSCALED:
	case XRFormat::B8G8R8A8_UINT:
	case XRFormat::B8G8R8A8_SINT:
	case XRFormat::B8G8R8A8_SRGB:
	case XRFormat::A8B8G8R8_UNORM_PACK32:
	case XRFormat::A8B8G8R8_SNORM_PACK32:
	case XRFormat::A8B8G8R8_USCALED_PACK32:
	case XRFormat::A8B8G8R8_SSCALED_PACK32:
	case XRFormat::A8B8G8R8_UINT_PACK32:
	case XRFormat::A8B8G8R8_SINT_PACK32:
	case XRFormat::A8B8G8R8_SRGB_PACK32:
	case XRFormat::A2R10G10B10_UNORM_PACK32:
	case XRFormat::A2R10G10B10_SNORM_PACK32:
	case XRFormat::A2R10G10B10_USCALED_PACK32:
	case XRFormat::A2R10G10B10_SSCALED_PACK32:
	case XRFormat::A2R10G10B10_UINT_PACK32:
	case XRFormat::A2R10G10B10_SINT_PACK32:
	case XRFormat::A2B10G10R10_UNORM_PACK32:
	case XRFormat::A2B10G10R10_SNORM_PACK32:
	case XRFormat::A2B10G10R10_USCALED_PACK32:
	case XRFormat::A2B10G10R10_SSCALED_PACK32:
	case XRFormat::A2B10G10R10_UINT_PACK32:
	case XRFormat::A2B10G10R10_SINT_PACK32:
		return 4;
	case XRFormat::R16_UNORM:
	case XRFormat::R16_SNORM:
	case XRFormat::R16_USCALED:
	case XRFormat::R16_SSCALED:
	case XRFormat::R16_UINT:
	case XRFormat::R16_SINT:
	case XRFormat::R16_SFLOAT:
		return 2;
	case XRFormat::R16G16_UNORM:
	case XRFormat::R16G16_SNORM:
	case XRFormat::R16G16_USCALED:
	case XRFormat::R16G16_SSCALED:
	case XRFormat::R16G16_UINT:
	case XRFormat::R16G16_SINT:
	case XRFormat::R16G16_SFLOAT:
		return 4;
	case XRFormat::R16G16B16_UNORM:
	case XRFormat::R16G16B16_SNORM:
	case XRFormat::R16G16B16_USCALED:
	case XRFormat::R16G16B16_SSCALED:
	case XRFormat::R16G16B16_UINT:
	case XRFormat::R16G16B16_SINT:
	case XRFormat::R16G16B16_SFLOAT:
		return 6;

	case XRFormat::R32G32_SFLOAT:
		return 8;

	case XRFormat::R32G32B32_SFLOAT:
		return 12;
	}
	assert(false);
	return -1;
}