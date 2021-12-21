#include "stdafx.h"
#include "XRPlatform.h"

#include "XRRenderer.h"
#include "XRObjectManager.h"

#include "XRSceneNode.h"
#include "XRTransformNode.h"
#include "XRLightNode.h"
#include "XRActorNode.h"

#include "XRCommandBuffer.h"
#include "XRPipeline.h"

static XRPipelineManager* GetDefaultPipelineManager()
{
	static XRPipelineManager _default;
	return &_default;
}

static XRWorkPassManager* GetDefaultWorkPassManager()
{
	static XRWorkPassManager _default;
	return &_default;
}

XRRenderer::XRRenderer()
	: _pipelineManager(GetDefaultPipelineManager())
	, _workPassManager(GetDefaultWorkPassManager())
	, _objectGroups()
{
}

XRRenderer::~XRRenderer()
{
	for (auto i : _objectGroups)
	{
		delete i.second;
	}
}

bool XRRenderer::Bind(XRPlatform::XRDSO* dso)
{
	std::vector<std::string> dllLists;
	XRPlatform::SymbolOption option;
	option._demangle = true;
	XRPlatform::ListDLLFunctions(dso, dllLists, option);

	for (uint32_t i = 0; i < dllLists.size(); ++i)
	{
		constexpr char const* head = "WorkPassAutoGenerator<";
		constexpr char const* tail = ">::generate";
		constexpr size_t lenHead = sizeof("WorkPassAutoGenerator<") - 1;

		size_t pos = dllLists[i].find(head);
		if (pos == std::string::npos)
			continue;

		pos = dllLists[i].find(">::generate", lenHead);
		if (pos == std::string::npos)
			continue;

		char const* name = dllLists[i].c_str();
		void* found = XRPlatform::GetProcAddress(dso, name);
		if (found == nullptr)
			continue;

		auto renderPassName = std::string(name + lenHead, pos - lenHead);
		_workPassManager->RegisterWorkPassGenerator(std::move(renderPassName), found);
	}

	return true;
}

XRCommandBuffer* XRRenderer::EvaluateCommands(XRCommandFootprint& commandFootprint)
{
	uint32_t currentFootprintHash = commandFootprint.MakeHash();
	XRCommandBuffer* commandBuffer = nullptr;

	auto found = _bakedCommandBuffers.find(currentFootprintHash);
	if (found != _bakedCommandBuffers.end())
	{
		commandBuffer = found->second;
	}
	else
	{
		commandBuffer = xrCreateCommandBuffer();

		commandBuffer->begin();
		{
			commandFootprint.Transcribe(commandBuffer);
		}
		commandBuffer->end();

		_bakedCommandBuffers.insert({ currentFootprintHash, commandBuffer });
	}

	return commandBuffer;
}

void XRRenderer::Update()
{
	OnUpdate();
}

void XRRenderer::Render()
{
	OnRender();

	++_renderCounter;
}

void XRRenderer::Reset()
{
	for (auto& i : _objectGroups)
	{
		XRObjectGroup* objectGroup = i.second;

		objectGroup->_objects.clear();
	}
}

void XRRenderer::RegisterNode(XRSceneNode* node)
{
	XRSceneNodeType type = node->GetType();
	switch (type)
	{
	case XRSceneNodeType::TRANSFORMATION:
		RegisterTransformNode(dynamic_cast<XRTransformNode*>(node));
		break;
	case XRSceneNodeType::LIGHT:
		RegisterLightNode(dynamic_cast<XRLightNode*>(node));
		break;
	case XRSceneNodeType::ACTOR:
		RegisterActorNode(dynamic_cast<XRActorNode*>(node));
		break;
	}
}

void XRRenderer::RegisterTransformNode(XRTransformNode* node)
{
	assert(false == _actorStack.empty());
	auto& lastActor = _actorStack.back();

	XRObjectGroup* objectGroup = lastActor.second;

	objectGroup->_objects.push_back(node);
}

void XRRenderer::RegisterLightNode(XRLightNode* node)
{

}

void XRRenderer::RegisterActorNode(XRActorNode* node)
{
	auto result = _objectGroups.insert({ node, nullptr });
	if (result.second == true)
	{
		result.first->second = new XRObjectGroup(node->getModel());
	}

	_actorStack.push_back({ node, result.first->second });
}