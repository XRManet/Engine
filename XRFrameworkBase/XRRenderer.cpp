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

static XRRenderPassManager* GetDefaultRenderPassManager()
{
	static XRRenderPassManager _default;
	return &_default;
}

static std::string sampleName = "teapot_1";

XRRenderer::XRRenderer()
	: _pipelineManager(GetDefaultPipelineManager())
	, _renderPassManager(GetDefaultRenderPassManager())
{
	_objectGroups[ sampleName ] = new XRObjectGroup;
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
		constexpr char const* head = "RenderPassAutoGenerator<";
		constexpr char const* tail = ">::generate";
		constexpr size_t lenHead = sizeof("RenderPassAutoGenerator<") - 1;

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
		_renderPassManager->RegisterRenderPassGenerator(std::move(renderPassName), found);
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
	XRObjectGroup* objectGroup = _objectGroups[ sampleName ];
	
	objectGroup->_objects.clear();
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

}

void XRRenderer::RegisterLightNode(XRLightNode* node)
{

}

void XRRenderer::RegisterActorNode(XRActorNode* node)
{
	XRObjectGroup* objectGroup = _objectGroups[ sampleName ];

	objectGroup->_objects.push_back(node);
}