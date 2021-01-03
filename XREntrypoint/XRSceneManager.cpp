#include "stdafx.h"
#include "XRSceneManager.h"

#include <XRFrameworkBase/XRScene.h>
#include <XRFrameworkBase/XRRenderer.h>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

char const* XRSceneManager::MANIFEST_NAME = "Resources/SceneKeys.json";

XRSceneManager::XRSceneManager()
{
	ParseManifest();

#if 0 // import symbol Ȯ�ο�
	for (auto& pair : _list_of_dsos)
	{
		auto dso = pair.second;

		std::vector<std::string> dllLists;
		XRPlatform::ListDLLFunctions(dso, dllLists);
	}
#endif
}

struct JsonLoader
{
	JsonLoader(const char* jsonPath)
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

	~JsonLoader()
	{
		if (read_stream_buffer)
			delete[] read_stream_buffer;

		fclose(fp_manifest);
	}

	void GetParsedDocument(rapidjson::Document& outDocument)
	{
		outDocument.ParseStream(read_stream);
	}

private:
	FILE* fp_manifest;
	char* read_stream_buffer;

	union {
		int __unused;
		rapidjson::FileReadStream read_stream;
	};
};

class XRSceneManagerManifestParser : public XRSceneManager
{
public:
	void RegisterScenes(rapidjson::Document::Array scenes)
	{
		for (auto& scene : scenes)
		{
			char const* key = scene["key"].GetString();
			char const* dso_name = scene["dso_name"].GetString();
			char const* func_name_CreateScene = scene["CreateScene"].GetString();
			auto renderers = scene["renderers"].GetArray();

			XRPlatform::XRDSO* dso;
			bool has_already = false;
			if (_list_of_dsos.find(dso_name) == _list_of_dsos.end())
			{
				dso = XRPlatform::LoadDSO(dso_name);
			}
			else
			{
				dso = _list_of_dsos[dso_name];
				has_already = true;
			}

			XRSceneInfo info{};
			info.dso_name = dso_name;
			info.CreateScene = reinterpret_cast<XRPFNCREATESCENE>(XRPlatform::GetProcAddress(dso, func_name_CreateScene));
			uint32_t numRenderers = renderers.Size();
			info.renderers.reserve(numRenderers);
			for (uint32_t i = 0; i < numRenderers; ++i)
			{
				info.renderers[i] = renderers[i].GetString();
			}

			if (info.IsAvailable() && _list_of_scenes.find(key) == _list_of_scenes.end())
			{
				_list_of_dsos[dso_name] = dso;

				// refresh dso_name
				info.dso_name = _list_of_dsos.find(dso_name)->first.c_str();

				_list_of_scenes[key] = std::move(info);
			}
			else if (has_already == false)
			{
			}
		}
	}

	void RegisterRenderers(rapidjson::Document::Array renderers)
	{
		for (auto& scene : renderers)
		{
			char const* key = scene["key"].GetString();
			char const* dso_name = scene["dso_name"].GetString();
			char const* func_name_CreateRenderer = scene["CreateRenderer"].GetString();

			XRPlatform::XRDSO* dso;
			bool has_already = false;
			if (_list_of_dsos.find(dso_name) == _list_of_dsos.end())
			{
				dso = XRPlatform::LoadDSO(dso_name);
			}
			else
			{
				dso = _list_of_dsos[dso_name];
				has_already = true;
			}

			XRRendererInfo info{};
			info.dso_name = dso_name;
			info.CreateRenderer = reinterpret_cast<XRPFNCREATERENDERER>(XRPlatform::GetProcAddress(dso, func_name_CreateRenderer));

			if (info.IsAvailable())
			{
				_list_of_dsos[dso_name] = dso;

				// refresh dso_name
				info.dso_name = _list_of_dsos.find(dso_name)->first.c_str();

				_list_of_renderers[key] = info;
			}
			else if (has_already == false)
			{
			}
		}
	}
};

void XRSceneManager::ParseManifest()
{
	JsonLoader sceneKeysLoader(XRSceneManager::MANIFEST_NAME);
	rapidjson::Document scene_keys_json;
	sceneKeysLoader.GetParsedDocument(scene_keys_json);
	
	XRSceneManagerManifestParser* parser = static_cast<XRSceneManagerManifestParser*>(this);
	parser->RegisterScenes(scene_keys_json["scenes"].GetArray());
	parser->RegisterRenderers(scene_keys_json["renderers"].GetArray());
}

XRSceneManager::~XRSceneManager()
{
	for (auto& dso : _list_of_dsos)
	{
		XRPlatform::UnloadDSO(dso.second);
	}
}

void XRSceneManager::QueryScenes(std::string&& query)
{

}

void XRSceneManager::BindPrimaryScene(std::string&& key)
{
	auto& sceneInfo = _list_of_scenes[key];
	_primary_scene = sceneInfo.GetScene();
	for (auto& i : sceneInfo.renderers)
	{
		_list_of_renderers[i].GetRenderer();
	}

	_current_renderer = _list_of_renderers[key].GetRenderer();
}

void XRSceneManager::InitializeRenderers(std::string&& sceneKey)
{
	for (auto& i : _list_of_scenes[sceneKey].renderers)
	{
		_list_of_renderers[i].GetRenderer();
	}
}
