#pragma once

#include <XRFrameworkBase/XRDefault.h>

class XRSceneNode;
class XRTransformNode;
class XRLightNode;
class XRActorNode;
class XRMaterialNode;

class XRScene;

class XRResourceManager;
class XRWorkPassManager;
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
	XRWorkPassManager* const _workPassManager;
	XRPipelineManager* const _pipelineManager;

	std::unordered_map<uint32_t, XRCommandBuffer*> _bakedCommandBuffers;

	std::unordered_map<XRActorNode*, XRObjectGroup*> _objectGroups;
	std::vector<std::pair<XRActorNode*, XRObjectGroup*>> _actorStack;

public:
	XRRenderer();
	~XRRenderer();

public:
	/**
	 * @brief	WorkPass interfaces
	 *
	 * @author	Jiman Jeong
	 * @date	2022-02-22
	 */

	bool Bind(XRPlatform::XRDSO* dso);

protected:
	/**
	 * @brief	Command interfaces
	 *
	 * @author	Jiman Jeong
	 * @date	2022-02-22
	 */
	bool EvaluateCommands(XRCommandFootprint& commandFootprint, std::vector<XRCommandBuffer*>& commandBuffers);

public:
	/**
	 * @brief	Renderer interfaces
	 *
	 * @author	Jiman Jeong
	 * @date	2022-02-22
	 */

	virtual void Initialize(XRResourceManager* resourceManager) {}

	virtual void WillUpdateRenderGraph(XRScene* scene) {}
	virtual void didUpdateRenderGraph(XRCommandBuffer* commandBuffer) {}
	virtual void OnRender() = 0;

public:
	void Update(XRScene* scene);
	void Render();

	uint64_t GetRenderCounter() const { return _renderCounter; }
	uint32_t GetAvailableBackBufferIndex();

public:
	/**
	 * @brief	SceneGraph to RHI MemoryLayout
	 *
	 * @author	Jiman Jeong
	 * @date	2022-02-22
	 */
	void BuildSceneMemoryLayout();

private:
	void BuildSceneMemoryLayout_StaticMesh();

public:
	/**
	 * @brief	SceneGraph Interfaces
	 *
	 * @author	Jiman Jeong
	 * @date	2022-01-19
	 */

	/**
	 * @fn	void Reset();
	 *
	 * @brief	우선은 object group 매프레임 정리하는데 씁니다.
	 *
	 * @author	Jiman Jeong
	 * @date	2021-02-19
	 */
	void Reset();

	void VisitNode(XRSceneNode* node);
	void LeaveNode(XRSceneNode* node);
	
private:
	void VisitTransformNode(XRTransformNode* node);
	void VisitLightNode(XRLightNode* node);
	void VisitActorNode(XRActorNode* node);
	void VisitMaterialNode(XRMaterialNode* node);

	void LeaveTransformNode(XRTransformNode* node);
	void LeaveLightNode(XRLightNode* node);
	void LeaveActorNode(XRActorNode* node);
	void LeaveMaterialNode(XRMaterialNode* node);
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