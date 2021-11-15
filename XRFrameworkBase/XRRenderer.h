#pragma once

#include <XRFrameworkBase/XRDefault.h>

class XRSceneNode;
class XRTransformNode;
class XRLightNode;
class XRActorNode;

class XRResourceManager;
class XRRenderPassManager;
class XRPipelineManager;

class XRCommandFootprint;
class XRCommandBuffer;

class XRObjectGroup;

namespace XRPlatform
{
struct XRDSO;
}

class XRBaseExport XRRenderer
{
private:
	uint64_t _renderCounter = 0;

protected:
	XRRenderPassManager* const _renderPassManager;
	XRPipelineManager* const _pipelineManager;

	std::unordered_map<uint32_t, XRCommandBuffer*> _bakedCommandBuffers;

	std::unordered_map<XRActorNode*, XRObjectGroup*> _objectGroups;
	std::vector<std::pair<XRActorNode*, XRObjectGroup*>> _actorStack;

public:
	XRRenderer();
	~XRRenderer();

public:
	bool Bind(XRPlatform::XRDSO* dso);

public:
	uint64_t GetRenderCounter() const { return _renderCounter; }

protected:
	XRCommandBuffer* EvaluateCommands(XRCommandFootprint& commandFootprint);

public:
	virtual void Initialize(XRResourceManager* resourceManager) {}

public:
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;

public:
	void Update();
	void Render();

public:

	/**
	 * @fn	void Reset();
	 *
	 * @brief	우선은 object group 매프레임 정리하는데 씁니다.
	 *
	 * @author	Jiman Jeong
	 * @date	2021-02-19
	 */

	void Reset();
	void RegisterNode(XRSceneNode* node);
	
private:
	void RegisterTransformNode(XRTransformNode* node);
	void RegisterLightNode(XRLightNode* node);
	void RegisterActorNode(XRActorNode* node);
};


using XRPFN_CREATE_RENDERER = XRRenderer * (*)();

struct XRRendererInfo
{
	char const* dso_name = nullptr;

	XRPFN_CREATE_RENDERER CreateRenderer = nullptr;

	XRRenderer* renderer = nullptr;

	bool isBound = false;

public:
	inline bool IsAvailable() { return dso_name != nullptr && CreateRenderer != nullptr; }

	XRRenderer* GetRenderer()
	{
		if (renderer == nullptr)
			renderer = CreateRenderer();
		return renderer;
	}


public:
	inline bool operator == (XRRendererInfo const& rendererInfo) {
		return dso_name == rendererInfo.dso_name
			&& CreateRenderer == rendererInfo.CreateRenderer;
	}

	inline bool operator != (XRRendererInfo const& rendererInfo) {
		return !(*this == rendererInfo);
	}
};