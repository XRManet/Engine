#pragma once

#include <vector>
#include <map>
#include <string>

#include <XRFrameworkBase/XRPlatform.h>

class XRScene;
struct XRSceneInfo;
struct XRRendererInfo;

class XRSceneManager
{
protected:
	std::map<std::string, XRSceneInfo> _list_of_scenes;
	std::map<std::string, XRRendererInfo> _list_of_renderers;
	std::map<std::string, XRPlatform::XRDSO*> _list_of_dsos;

private:
	XRScene* _primary_scene = nullptr;
	XRRenderer* _current_renderer = nullptr;


private:
	XRSceneManager();
	~XRSceneManager();

private:
	void ParseManifest();

public:
	static XRSceneManager* GetInstance() {
		static XRSceneManager _instance;

		return &_instance;
	}

	static char const* MANIFEST_NAME;
	static char const* MANIFEST_RENDERER_KEYS_NAME;

public:
	void QueryScenes(std::string&& query);


public:
	void BindPrimaryScene(std::string&& sceneKey);
	void InitializeRenderers(std::string&& sceneKey);

public:
	XRScene* GetPrimaryScene() { return _primary_scene; }
	XRRenderer* GetCurrentRenderer() { return _current_renderer; }
};