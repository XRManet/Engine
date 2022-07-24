#include "stdafx.h"
#include "XRTextureGL.h"

#include <XRFrameworkBase/XRTextureData.h>

struct GLFormatInfo
{
	GLuint internal_format = GL_NONE;
	GLuint blockSize = 0;
	GLuint swizzle_format = GL_NONE;
};

static GLFormatInfo GetTextureFormatFromXR(XRTextureFormat format)
{
	switch (format)
	{
	case XRTextureFormat::DXT1: // 4x4 pixels / block
		return { GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 8 };
	case XRTextureFormat::DXT3: // 4x4 pixels / block
		return { GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 16 };
	case XRTextureFormat::DXT5: // 4x4 pixels / block
		return { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 16 };
	case XRTextureFormat::RGBA_8888: // pixel / block
		return { GL_RGBA, 4, GL_RGBA };
	case XRTextureFormat::BGRA_8888: // pixel / block
		return { GL_RGBA, 4, GL_BGRA };
	case XRTextureFormat::RGB_888: // pixel / block
		return { GL_RGBA, 3, GL_RGB };
	case XRTextureFormat::BGR_888: // pixel / block
		return { GL_RGBA, 3, GL_BGR };
	};
	assert("Unknown format uses" == nullptr);
	return { GL_NONE, 0 };
}

static GLuint GetTextureTypeFromXR(XRTextureType type)
{
	switch (type)
	{
	case XRTextureType::Tex1D:
		return GL_TEXTURE_1D;
	case XRTextureType::Tex1DArray:
		return GL_TEXTURE_1D_ARRAY;
	case XRTextureType::Tex2D:
		return GL_TEXTURE_2D;
	case XRTextureType::Tex2DArray:
		return GL_TEXTURE_2D_ARRAY;
	case XRTextureType::Tex3D:
		return GL_TEXTURE_3D;
	case XRTextureType::TexCube:
		return GL_TEXTURE_CUBE_MAP;
	case XRTextureType::TexCubeArray:
		return GL_TEXTURE_CUBE_MAP_ARRAY;
	case XRTextureType::Tex2DMS:
		return GL_TEXTURE_2D_MULTISAMPLE;
	case XRTextureType::Tex2DMSArray:
		return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
	}
	assert("Unknown type uses" == nullptr);
	return GL_NONE;
}

static uint32_t GetBlockLength(uint32_t length)
{
	return length + 3 / 4;
}

void XRTextureGL::upload(XRTextureData const* data)
{
	auto* header = data->GetHeader();
	auto* buffer = static_cast<uint8_t const*>(data->GetData());

	assert(header->num_mipmaps > 0);

	GL_CALL(glGenTextures(1, &_texture));

	// Todo(jiman): Type에 따라서 target 텍스쳐 결정할 수 있도록.
	GLuint target_type = GL_TEXTURE_2D;// GetTextureTypeFromXR(header->type);
	GL_CALL(glBindTexture(target_type, _texture));

	GLFormatInfo formatInfo = GetTextureFormatFromXR(header->format);

	uint32_t offset = 0;
	uint32_t width = header->width;
	uint32_t height = header->height;

	GL_CALL(glTexParameteri(target_type, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CALL(glTexParameteri(target_type, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GL_CALL(glTexParameteri(target_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CALL(glTexParameteri(target_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

	if (header->num_mipmaps == 1)
	{
		GL_CALL(glTexImage2D(target_type, 0, formatInfo.internal_format, width, height, 0, formatInfo.swizzle_format, GL_UNSIGNED_BYTE, buffer));
		GL_CALL(glGenerateMipmap(target_type));
	}
	else
	{
		for (uint32_t level = 0; level < header->num_mipmaps && (width || height); ++level)
		{
			uint32_t size = GetBlockLength(width) * GetBlockLength(height) * formatInfo.blockSize;
			GL_CALL(glCompressedTexImage2D(target_type, level, formatInfo.internal_format, width, height,
				0, size, buffer + offset));

			offset += size;
			width /= 2;
			height /= 2;
		}
	}
}

XRTextureGL::~XRTextureGL()
{
}
