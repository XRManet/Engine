#include "stdafx.h"

#include "XRDXTC.h"

bool XRDXTC::LoadDataFromFile()
{
	const uint32_t DDS_SIGN_SIZE = 4;
	const uint32_t DDS_HEADER_SIZE_EXCEPT_SIGN = 124;
	unsigned char header[DDS_HEADER_SIZE_EXCEPT_SIGN];

	FILE *fp = nullptr;
	errno_t error = fopen_s(&fp, GetPath().c_str(), "rb");
	if (error != 0 || fp == nullptr)
		return false;

	char filecode[DDS_SIGN_SIZE];
	fread(filecode, 1, DDS_SIGN_SIZE, fp);
	if (strncmp(filecode, "DDS ", DDS_SIGN_SIZE) != 0)
	{
		fclose(fp);
		return false;
	}

	fread(&header, DDS_HEADER_SIZE_EXCEPT_SIGN, 1, fp);

	TextureHeader *tex_header = GetHeader();
	tex_header->height = reinterpret_cast<uint32_t&>(header[8]);
	tex_header->width = reinterpret_cast<uint32_t&>(header[12]);
	tex_header->size_in_bytes = reinterpret_cast<uint32_t&>(header[16]);
	tex_header->num_mipmaps = reinterpret_cast<uint32_t&>(header[24]);
	tex_header->format = reinterpret_cast<XRTextureFormat&>(header[80]);

	if (tex_header->num_mipmaps > 1)
		tex_header->size_in_bytes *= 2;

	size_t header_size = _memory.size();
	_memory.resize(header_size + tex_header->size_in_bytes);
	fread(_memory.data() + header_size, 1, tex_header->size_in_bytes, fp);

	fclose(fp);

	return true;
}