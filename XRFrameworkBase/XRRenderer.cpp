﻿#include "stdafx.h"
#include "XRPlatform.h"

#include "XRRenderer.h"
#include "XRObjectManager.h"

#include "XRModel.h"

#include "XRSceneNode.h"
#include "XRTransformNode.h"
#include "XRLightNode.h"
#include "XRActorNode.h"

#include "XRCommandBuffer.h"
#include "XRPipeline.h"

#include "XRBuffer.h"

static XRPipelineManager* GetDefaultPipelineManager()
{
	static XRPipelineManager _default(nullptr);
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

bool XRRenderer::EvaluateCommands(XRCommandFootprint& commandFootprint, std::vector<XRCommandBuffer*>& commandBuffers)
{
	// Idea:
	// Preprocess를 통해 dependency graph를 선행 분석
	// 분석된 flow 마다 분리된 command buffer를 할당하고 기록하도록 처리
	// 해당은 충분히 병렬 처리할 수 있을 것.
	// 병렬화된 pass 실행까지 여기서 정리.
	commandFootprint.Preprocess();

	uint32_t currentFootprintHash = commandFootprint.MakeHash();
	
	// dependency graph가 마련되기 전까진 n개 출력 처리해둘 것.
	XRCommandBuffer* commandBuffer = nullptr;

	auto found = _bakedCommandBuffers.find(currentFootprintHash);
	if (found != _bakedCommandBuffers.end())
	{
		commandBuffer = found->second;
	}
	else
	{
		commandBuffer = xrCreateCommandBuffer(nullptr);

		commandBuffer->begin();
		{
			commandFootprint.Transcribe(commandBuffer);
		}
		commandBuffer->end();

		_bakedCommandBuffers.insert({ currentFootprintHash, commandBuffer });
	}

	commandBuffers.push_back(commandBuffer);
	return true;
}

void XRRenderer::Update(XRScene* scene)
{
	BuildSceneMemoryLayout();

	WillUpdateRenderGraph(scene);

	for (auto& i : _objectGroups)
	{
		XRObjectGroup* objectGroup = i.second;
	}

	XRCommandBuffer* commandBuffer = xrCreateCommandBuffer(nullptr);
	commandBuffer->begin();

	didUpdateRenderGraph(commandBuffer);

	commandBuffer->end();
	commandBuffer->executeCommands();

	delete commandBuffer;
}

void XRRenderer::Render()
{
	OnRender();

	++_renderCounter;
}

uint32_t XRRenderer::GetAvailableBackBufferIndex()
{
	// Todo(jiman): Swapchain 과의 연결 관계 설정 필요. #Todo_Swapchain
	return 0;
}

struct XRPrimitiveBuffer
{
	static const uint32_t MaxVertexBuffers = 4;

	XRBuffer* _vertexBuffers[MaxVertexBuffers]{};
	XRBuffer* _indexBuffer{};
};

class XRLoadable;
struct XRPrimitiveDescription
{
	std::vector<xr::IndexedString<XRLoadable, uint32_t>> _meshNames;
	uint32_t _vertexCount{};
	uint32_t _indexCount{};
};

// XRMutablePrimitive 가 더 맞음
struct XRPrimitive
{
	XRPrimitiveDescription _description{};
	XRPrimitiveBuffer _buffers{};
};

/**
 * @fn	void XRRenderer::BuildSceneMemoryLayout()
 *
 * @brief	input layout
 * 				mesh
 * 					per-mesh materials
 * 						instance
 * 							instance buffer per-instance materials
 * 						sub-mesh
 * 							vertex buffers index buffer per-sub-mesh materials
 *
 * @author	Jiman Jeong
 * @date	2022-02-23
 */

void XRRenderer::BuildSceneMemoryLayout()
{
	BuildSceneMemoryLayout_StaticMesh();
}

void XRRenderer::BuildSceneMemoryLayout_StaticMesh()
{
	std::unordered_map<XRInputLayout const*, XRPrimitive> __map;
	std::unordered_map<XRInputLayout const*, XRPrimitiveDescription> __compare;

	for (auto& i : _objectGroups)
	{
		XRObjectGroup* objectGroup = i.second;
		XRModel const* model = objectGroup->_model;
		XRInputLayout const* inputLayout = model->getInputLayout();

		auto result = __compare.insert({ inputLayout, {} });
		XRPrimitiveDescription& primitiveDescription = result.first->second;
		primitiveDescription._vertexCount += model->getNumVertices();
		primitiveDescription._indexCount += model->getNumIndices();
	}

	// 이번에 추가된 primitive와 이전 것을 비교, 변경점이 있으면 background 갱신 처리가 들어가야 한다.
	for (auto& i : _objectGroups)
	{
		XRObjectGroup* objectGroup = i.second;
		XRModel const* model = objectGroup->_model;
		XRInputLayout const* inputLayout = model->getInputLayout();


		auto result = __map.insert({ inputLayout, {} });
		auto found = __compare.find(inputLayout);

		XRPrimitiveDescription& curPrimitiveDescription = result.first->second._description;
		XRPrimitiveDescription& newPrimitiveDescription = found->second;
		if ((curPrimitiveDescription._vertexCount != newPrimitiveDescription._vertexCount)
			|| (curPrimitiveDescription._indexCount != newPrimitiveDescription._indexCount))
		{
			
		}

	}
}

void XRRenderer::Reset()
{
	for (auto& i : _objectGroups)
	{
		XRObjectGroup* objectGroup = i.second;

		objectGroup->_objects.clear();
	}
}

void XRRenderer::VisitNode(XRSceneNode* node)
{
	XRSceneNodeType type = node->GetType();
	switch (type)
	{
	case XRSceneNodeType::TRANSFORMATION:
		VisitTransformNode(static_cast<XRTransformNode*>(node));
		break;
	case XRSceneNodeType::LIGHT:
		VisitLightNode(static_cast<XRLightNode*>(node));
		break;
	case XRSceneNodeType::ACTOR:
		VisitActorNode(static_cast<XRActorNode*>(node));
		break;
	}
}

void XRRenderer::VisitTransformNode(XRTransformNode* node)
{
	assert(false == _actorStack.empty());
	auto& lastActor = _actorStack.back();

	XRObjectGroup* objectGroup = lastActor.second;

	objectGroup->_objects.push_back(node);
}

void XRRenderer::VisitLightNode(XRLightNode* node)
{

}

void XRRenderer::VisitActorNode(XRActorNode* node)
{
	auto result = _objectGroups.insert({ node, nullptr });
	if (result.second == true)
	{
		result.first->second = new XRObjectGroup(node->getModel());
	}

	_actorStack.push_back({ node, result.first->second });
}

void XRRenderer::VisitMaterialNode(XRMaterialNode* node)
{
}

void XRRenderer::LeaveNode(XRSceneNode* node)
{
	XRSceneNodeType type = node->GetType();
	switch (type)
	{
	case XRSceneNodeType::TRANSFORMATION:
		LeaveTransformNode(static_cast<XRTransformNode*>(node));
		break;
	case XRSceneNodeType::LIGHT:
		LeaveLightNode(static_cast<XRLightNode*>(node));
		break;
	case XRSceneNodeType::ACTOR:
		LeaveActorNode(static_cast<XRActorNode*>(node));
		break;
	}
}

void XRRenderer::LeaveTransformNode(XRTransformNode* node)
{

}

void XRRenderer::LeaveLightNode(XRLightNode* node)
{

}

void XRRenderer::LeaveActorNode(XRActorNode* node)
{
	auto& lastActor = _actorStack.back();
	assert(lastActor.first == node);

	_actorStack.pop_back();
}

void XRRenderer::LeaveMaterialNode(XRMaterialNode* node)
{

}