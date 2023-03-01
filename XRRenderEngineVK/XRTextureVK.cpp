#include "stdafx.h"
#include "XRTextureVK.h"

#include <XRFrameworkBase/XRTextureData.h>

#include <glm/vec4.hpp>

struct VkFormatInfo
{
	VkFormat format;
	uint32_t blockSize = 0;
};

static VkFormatInfo GetTextureFormatFromXR(XRTextureFormat format)
{
	switch (format)
	{
	case XRTextureFormat::DXT1: // 4x4 pixels / block
		return { VK_FORMAT_BC1_RGBA_UNORM_BLOCK, 8 };
	case XRTextureFormat::DXT3: // 4x4 pixels / block
		return { VK_FORMAT_BC2_UNORM_BLOCK, 16 };
	case XRTextureFormat::DXT5: // 4x4 pixels / block
		return { VK_FORMAT_BC3_UNORM_BLOCK, 16 };
	case XRTextureFormat::RGBA_8888: // pixel / block
		return { VK_FORMAT_R8G8B8A8_UNORM, 4 };
	case XRTextureFormat::BGRA_8888: // pixel / block
		return { VK_FORMAT_B8G8R8A8_UNORM, 4 };
	case XRTextureFormat::RGB_888: // pixel / block
		return { VK_FORMAT_R8G8B8_UNORM, 3 };
	case XRTextureFormat::BGR_888: // pixel / block
		return { VK_FORMAT_B8G8R8_UNORM, 3 };
	};
	assert("Unknown format uses" == nullptr);
	return { VK_FORMAT_UNDEFINED, 0 };
}

static VkImageType GetTextureTypeFromXR(XRTextureType type)
{
	switch (type)
	{
	case XRTextureType::Tex1D:
		return VK_IMAGE_TYPE_1D;
	case XRTextureType::Tex1DArray:
		return VK_IMAGE_TYPE_1D;
	case XRTextureType::Tex2D:
		return VK_IMAGE_TYPE_2D;
	case XRTextureType::Tex2DArray:
		return VK_IMAGE_TYPE_2D;
	case XRTextureType::Tex3D:
		return VK_IMAGE_TYPE_3D;
	case XRTextureType::TexCube:
		return VK_IMAGE_TYPE_2D;
	case XRTextureType::TexCubeArray:
		return VK_IMAGE_TYPE_2D;
	case XRTextureType::Tex2DMS:
		return VK_IMAGE_TYPE_2D;
	case XRTextureType::Tex2DMSArray:
		return VK_IMAGE_TYPE_2D;
	}
	assert("Unknown type uses" == nullptr);
	return VK_IMAGE_TYPE_MAX_ENUM;
}

static uint32_t GetBlockLength(uint32_t length)
{
	return length + 3 / 4;
}

void XRTextureVK::upload(XRTextureData const* data)
{
	auto* header = data->GetHeader();
	auto* buffer = static_cast<uint8_t const*>(data->GetData());

	//assert(header->num_mipmaps > 0);

	//GL_CALL(glGenTextures(1, &_texture));

	//// Todo(jiman): Type에 따라서 target 텍스쳐 결정할 수 있도록.
	//GLuint target_type = GL_TEXTURE_2D;// GetTextureTypeFromXR(header->type);
	//GL_CALL(glBindTexture(target_type, _texture));

	//VkFormatInfo formatInfo = GetTextureFormatFromXR(header->format);

	//uint32_t offset = 0;
	//uint32_t width = header->width;
	//uint32_t height = header->height;

	//GL_CALL(glTexParameteri(target_type, GL_TEXTURE_WRAP_S, GL_REPEAT));
	//GL_CALL(glTexParameteri(target_type, GL_TEXTURE_WRAP_T, GL_REPEAT));
	//GL_CALL(glTexParameteri(target_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//GL_CALL(glTexParameteri(target_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

	//if (header->num_mipmaps == 1)
	//{
	//	GL_CALL(glTexImage2D(target_type, 0, formatInfo.internal_format, width, height, 0, formatInfo.swizzle_format, GL_UNSIGNED_BYTE, buffer));
	//	GL_CALL(glGenerateMipmap(target_type));
	//}
	//else
	//{
	//	for (uint32_t level = 0; level < header->num_mipmaps && (width || height); ++level)
	//	{
	//		uint32_t size = GetBlockLength(width) * GetBlockLength(height) * formatInfo.blockSize;
	//		GL_CALL(glCompressedTexImage2D(target_type, level, formatInfo.internal_format, width, height,
	//			0, size, buffer + offset));

	//		offset += size;
	//		width /= 2;
	//		height /= 2;
	//	}
	//}
}

XRTextureVK::~XRTextureVK()
{
}
