#include "stdafx.h"
#include "XRJson.h"

JsonLoader::JsonLoader(const char* jsonPath)
	: fp_manifest(nullptr)
	, read_stream_buffer(nullptr)
{
	errno_t error = xr::fopen(&fp_manifest, jsonPath, "rb");
	if (error != 0 || fp_manifest == nullptr)
		throw;

	int const len_manifest = (static_cast<void>(fseek(fp_manifest, 0, SEEK_END)), static_cast<int>(ftell(fp_manifest)));
	rewind(fp_manifest);

	char* read_stream_buffer = new char[len_manifest];
	read_stream = rapidjson::FileReadStream(fp_manifest, read_stream_buffer, len_manifest);
}

JsonLoader::~JsonLoader()
{
	if (read_stream_buffer)
		delete[] read_stream_buffer;

	fclose(fp_manifest);
}

void JsonLoader::GetParsedDocument(rapidjson::Document& outDocument)
{
	outDocument.ParseStream(read_stream);
}