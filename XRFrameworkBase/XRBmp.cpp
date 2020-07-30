#include "stdafx.h"

#include "XRBmp.h"

bool XRBmp::LoadDataFromFile()
{
	const uint32_t BMP_SIGN_SIZE = 2;
	const uint32_t BMP_HEADER_SIZE = 54;
	unsigned char header[BMP_HEADER_SIZE];

	FILE *fp = nullptr;
	errno_t error = fopen_s(&fp, GetPath().c_str(), "rb");
	if (error != 0 || fp == nullptr)
		return false;

	size_t read_count = fread(header, 1, BMP_HEADER_SIZE, fp);
	if (read_count < BMP_HEADER_SIZE || (header[0] == 'B' && header[1] == 'M') == false)
	{
		fclose(fp);
		return false;
	}

	TextureHeader *tex_header = GetHeader();
	tex_header->width = reinterpret_cast<uint32_t&>(header[0x12]);
	tex_header->height = reinterpret_cast<uint32_t&>(header[0x16]);
	tex_header->size_in_bytes = reinterpret_cast<uint32_t&>(header[0x22]);
	
	// No data.
	tex_header->format = XRTextureFormat::BGR_888;
	tex_header->channels = 3;
	tex_header->type = XRTextureType::Tex2D;
	tex_header->num_mipmaps = 1;

	if (tex_header->size_in_bytes == 0)
		tex_header->size_in_bytes = tex_header->width * tex_header->height * 3;

	size_t header_size = _memory.size();
	_memory.resize(header_size + tex_header->size_in_bytes);
	fread(_memory.data() + header_size, 1, tex_header->size_in_bytes, fp);

	fclose(fp);

	return true;
}