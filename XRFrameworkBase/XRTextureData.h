#pragma once

#include <stdafx.h>

#include "XRLoadable.h"
#include "XRPrimitiveTypes.h"

enum class XRTextureFormat : uint32_t
{
	Unknown = 0,
	RGB_888 = 1,
	BGR_888 = 2,
	RGBA_8888 = 3,
	BGRA_8888 = 4,
	ARGB_8888 = 5,
	ABGR_8888 = 6,
	DXT1 = 0x31545844, // "DXT1" in ASCII
	DXT3 = 0x33545844, // "DXT3" in ASCII
	DXT5 = 0x35545844, // "DXT5" in ASCII
};

enum class XRTextureType : uint32_t
{
	Unknown,
	Tex1D,
	Tex1DArray,
	Tex2D,
	Tex2DArray,
	Tex3D,
	TexCube,
	TexCubeArray,
	Tex2DMS,
	Tex2DMSArray,
};

class XRTextureData : public XRLoadable
{
public:
	struct TextureHeader
	{
		int32_t width = 0;
		int32_t height = 0;
		int32_t depth = 0;
		int32_t size_in_bytes = 0;
		int8_t bits_r = 0;
		int8_t bits_g = 0;
		int8_t bits_b = 0;
		int8_t bits_a = 0;
		uint32_t channels = 0;
		uint32_t num_mipmaps = 0;
		XRTextureFormat format = XRTextureFormat::Unknown;
		XRTextureType type = XRTextureType::Unknown;
		uint32_t data_offset = 0;
	};

public:
	XRTextureData(std::string && path) : XRLoadable(std::move(path)) { Initialize(); }
	XRTextureData(std::string const& path) : XRLoadable(path) { Initialize(); }

private:
	void Initialize();

public:		void const*				GetData() const { return _memory.data() + GetHeader()->data_offset; }
public:		TextureHeader const*	GetHeader() const { return reinterpret_cast<TextureHeader const*>(_memory.data()); }
protected:	TextureHeader *			GetHeader() { return reinterpret_cast<TextureHeader*>(_memory.data()); }
};
