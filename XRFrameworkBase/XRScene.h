#pragma once

#include <XRFrameworkBase/XRDefault.h>

#include "XRObjectManager.h"
#include "XRResourceManager.h"
#include "XRSceneNode.h"

class XRCamera;
class XRRenderer;

class XRPipelineManager;
class XRBaseExport XRScene
{
protected:
	XRObjectManager _object_manager;
	XRResourceManager _resource_manager;
	std::vector<XRCamera> _cameras;
	XRSceneNode* _root;

public:
	std::vector<XRCamera>& getCameras() { return _cameras; }

public:
	XRScene();
	virtual ~XRScene();

public:
	XRResourceManager* GetResourceManager() { return &_resource_manager; }

public:
	// 왠지 이런게 필요할거같긴한데...
	XRSceneNode* GenerateNode(XRSceneNodeType type);

public:
	virtual void Initialize() {}
	virtual void Update(float dt) {}
	void Render(XRRenderer* renderer);

public:
	virtual void PrepareToWakeUp() {}
	virtual void ReadyToBePaused() {}
};

using XRPFNCREATESCENE = XRScene * (*)();

struct XRSceneInfo
{
	char const* dso_name = nullptr;

	XRPFNCREATESCENE CreateScene = nullptr;
	XRScene* scene = nullptr;
	std::vector<std::string> renderers;

public:
	inline bool IsAvailable() { return dso_name != nullptr && CreateScene != nullptr; }

public:
	XRScene* GetScene()
	{
		if (scene == nullptr)
			scene = CreateScene();
		return scene;
	}

public:
	inline bool operator == (XRSceneInfo const& sceneInfo) {
		return dso_name == sceneInfo.dso_name
			&& CreateScene == sceneInfo.CreateScene;
	}

	inline bool operator != (XRSceneInfo const& sceneInfo) {
		return !(*this == sceneInfo);
	}
};