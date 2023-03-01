#pragma once

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

struct XRBaseExport JsonLoader
{
	JsonLoader(const char* jsonPath);
	~JsonLoader();

	void GetParsedDocument(rapidjson::Document& outDocument);

private:
	FILE* fp_manifest;
	char* read_stream_buffer;

	union {
		int __unused;
		rapidjson::FileReadStream read_stream;
	};
};