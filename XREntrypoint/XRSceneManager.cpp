#include "stdafx.h"
#include "XRSceneManager.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

char const* XRSceneManager::MANIFEST_NAME = "Resources/SceneKeys.json";

XRSceneManager::XRSceneManager()
{
	FILE *fp_manifest = nullptr;
	errno_t error = xr::fopen(&fp_manifest, XRSceneManager::MANIFEST_NAME, "rb");
	if (error != 0 || fp_manifest == nullptr)
		throw;

	int const len_manifest = (static_cast<void>(fseek(fp_manifest, 0, SEEK_END)), static_cast<int>(ftell(fp_manifest)));
	rewind(fp_manifest);

	char * read_stream_buffer = new char[len_manifest];
	rapidjson::FileReadStream read_stream(fp_manifest, read_stream_buffer, len_manifest);

	rapidjson::Document scene_keys_json;
	scene_keys_json.ParseStream(read_stream);

	for (auto& scene : scene_keys_json["scenes"].GetArray())
	{
		char const* key = scene["key"].GetString();
		char const* dso_name = scene["dso_name"].GetString();
		char const* func_name_CreateScene = scene["CreateScene"].GetString();

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

		if (info.IsAvailable())
		{
			_list_of_scenes[key] = info;
			_list_of_dsos[dso_name] = dso;

			// refresh dso_name
			info.dso_name = _list_of_dsos.find(dso_name)->first.c_str();

			std::vector<std::string> dllLists;
			XRPlatform::ListDLLFunctions(dso, dllLists);

			//XRRenderPassBase* ;
		}
		else if (has_already == false)
		{
			XRPlatform::UnloadDSO(dso);
		}
	}

	delete[] read_stream_buffer;
	fclose(fp_manifest);
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
	_primary_scene = _list_of_scenes[key].CreateScene();
}
