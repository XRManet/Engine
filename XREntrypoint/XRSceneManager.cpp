#include "stdafx.h"
#include "XRSceneManager.h"

#include <XRFrameworkBase/XRJson.h>
#include <XRFrameworkBase/XRScene.h>
#include <XRFrameworkBase/XRRenderer.h>

char const* XRSceneManager::MANIFEST_NAME = "Resources/SceneKeys.json";

XRSceneManager::XRSceneManager()
{
	ParseManifest();

#if 0 // import symbol 확인용
	for (auto& pair : _list_of_dsos)
	{
		auto dso = pair.second;

		std::vector<std::string> dllLists;
		XRPlatform::ListDLLFunctions(dso, dllLists);
	}
#endif
}

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
			info.renderers.resize(numRenderers);
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
			info.CreateRenderer = reinterpret_cast<XRPFN_CREATE_RENDERER>(XRPlatform::GetProcAddress(dso, func_name_CreateRenderer));

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

void XRSceneManager::QueryScenes(std::string const& query)
{

}

void XRSceneManager::BindPrimaryScene(std::string const& key)
{
	auto& sceneInfo = _list_of_scenes[key];
	_primary_scene = sceneInfo.GetScene();

	BindRenderer(sceneInfo.renderers[0]);
}

void XRSceneManager::BindRenderer(std::string const& rendererKey)
{
	auto& rendererInfo = _list_of_renderers[rendererKey];
	_current_renderer = rendererInfo.GetRenderer();

	if (false == rendererInfo.isBound)
	{
		rendererInfo.isBound = _current_renderer->Bind(_list_of_dsos[rendererInfo.dso_name]);
	}
}

void XRSceneManager::InitializeSceneRenderers(std::string const& sceneKey)
{
	for (auto& i : _list_of_scenes[sceneKey].renderers)
	{
		_list_of_renderers[i].GetRenderer();
	}
}
