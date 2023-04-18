﻿#include "stdafx.h"

#include "XRRendererTest.h"

#include <XRFrameworkBase/XRUtility.h>
#include <XRFrameworkBase/XRIndexedString.h>
#include <XRFrameworkBase/XRScene.h>

#include <XRFrameworkBase/XRResourceManager.h>
#include <XRFrameworkBase/XRObjectManager.h>
#include <XRFrameworkBase/XRModel.h>


#include <XRFrameworkBase/XRCommandBuffer.h>
#include <XRFrameworkBase/XRPipeline.h>
#include <XRFrameworkBase/XRTexture.h>

template<typename Permutation, int Count>
struct PipelineKeyImpl : public PipelineKeyImpl<Permutation, Count - 1>
{
};

template<typename Permutation>
struct PipelineKeyImpl<Permutation, 0>
{
};

template<typename Permutation> struct PermutationDescription;

struct Default {};
template<typename Permutation>
struct PipelineKey : public PipelineKeyImpl<Permutation, Permutation::COUNT>
{
	PipelineKey(typename PermutationDescription<Permutation>::Type&& permutation)
	{
	}

	PipelineKey(Default) {}
};

struct PermutationEnum
{
	char const* _id;
};

#define PermutationEnumStaticEnumCtor(id, index) { {#id} }
#define PermutationElementInfoCtor(name, count) { {#name, count} }

STATIC_ENUM_BEGIN(SamplePermutation, XRPermutationBase);
ADD_ENUM(SamplePermutation, AlphaTest, 2);
ADD_ENUM(SamplePermutation, Lod, 2);
STATIC_ENUM_END(SamplePermutation);

template<>
struct PermutationDescription<SamplePermutation>
{
	using Type = std::tuple<bool, int>;
};

class WorkPass(XRWorkPassPresent)
{
private:
	virtual void DefaultPipelineState(XRPipelineStateDescription & outDefault) override {}
};

class WorkPass(XRWorkPassSample)
{
private: // Common-Pipeline descriptions;
	XRInputLayout const* _inputLayout;

	friend XRRendererTest;

private: // Per-Pipeline descriptions;
	XRShaderStageDescription _defaultShaderDescription;
	XRShaderStageDescription _alphaShaderDescription;
	XRVertexInputStateDescription _defaultVertexInputStateDescription;

public:
	XRWorkPassSample()
		//: _inputLayout(inputLayout)
	{
		//Create RenderPass
		//	Define Attachment Descriptions
		//	Define Subpasses

		//	Define PipelineStateDescriptions
		_pipelineCreateInfos.resize(1);
		{
			XRPipelineCreateInfo& pipelineCreateInfo = _pipelineCreateInfos[0];
			pipelineCreateInfo._name = xr::IndexedString<XRPipeline>("SampleFirstPipeline");

			const char* CommonVertex = "SimpleVertex.glsl";
			_defaultShaderDescription._vertexFilename = CommonVertex;
			_defaultShaderDescription._fragmentFilename = "SimpleFragment.glsl";
			_alphaShaderDescription._vertexFilename = CommonVertex;
			_alphaShaderDescription._fragmentFilename = "SimpleFragmentAlpha.glsl";
			pipelineCreateInfo._description._shaderStageDescription = &_defaultShaderDescription;

			_defaultVertexInputStateDescription.init(_inputLayout, XRPrimitiveTopology::TriangleList);
			pipelineCreateInfo._description._vertexInputStateDescription = &_defaultVertexInputStateDescription;

			pipelineCreateInfo._permutationLayout = SamplePermutation::GetLayout();
			pipelineCreateInfo._permute = [this](XRPipelineStateDescription& outDescription, std::vector<XRPermutationElementArgument> const& elements)
			{
				for (uint32_t i = 0; i < elements.size(); ++i)
				{
					SamplePermutation const& sample = static_cast<SamplePermutation const&>(elements[i]._id);
					sample->_name == "AlphaTest" || sample->_name == "Lod";
				}

				if (elements[SamplePermutation::AlphaTest]._value == 1)
				{
					outDescription._shaderStageDescription = &_alphaShaderDescription;
				}

				return true;
			};
		}
	}

private:
	virtual void DefaultPipelineState(XRPipelineStateDescription & outDefault) override
	{

	}
};

struct MatrixBlock
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewProj;
};

struct LightBlock
{
	glm::vec4 position;
	glm::vec3 intensity;
	float attenuation;
};

XRRendererTest::XRRendererTest()
{
	// TODO) Initialize()의 내용을 glew 초기화 특정되는 순간 할 수 있는게 좋음.
	// 1. Observer를 둬서 라이브러리 준비가 끝나면 Initialize()가 호출되도록,
	// 2. 아니면 람다를 등록해서 라이브러리 준비가 끝나면 그 즉시 호출되도록
}

XRRendererTest::~XRRendererTest()
{
}

void XRRendererTest::Initialize(XRResourceManager* resourceManager)
{
	static xr::IndexedString<XRWorkPassBase> workPassSampleName = "XRWorkPassSample";
	XRWorkPassSample* renderPassSample = static_cast<XRWorkPassSample*>(_workPassManager->GetWorkPass(workPassSampleName));

	_pipelineManager->CreatePipeline(renderPassSample->_pipelineCreateInfos[0]);

	XRBufferCreateInfo bufferCreateInfoPerFrame = {
		._size = sizeof(MatrixBlock) + sizeof(LightBlock)
	};
	_bufferPerFrame = xrCreateBuffer(&bufferCreateInfoPerFrame);
}

void XRRendererTest::WillUpdateRenderGraph(XRScene* scene)
{
	{
		const size_t offsetMatrixBlock = 0;
		const size_t sizeMatrixBlock = (sizeof(MatrixBlock) * 1);
		const size_t offsetLightBlock = offsetMatrixBlock + sizeMatrixBlock;
		const size_t sizeLightBlock = (sizeof(LightBlock) * 1);
		_uniformBufferData.resize(sizeMatrixBlock + sizeLightBlock);

		MatrixBlock& matrixBlock = reinterpret_cast<MatrixBlock&>(_uniformBufferData[offsetMatrixBlock]);
		LightBlock& lightBlock = reinterpret_cast<LightBlock&>(_uniformBufferData[offsetLightBlock]);

		matrixBlock.view = scene->getCameras()[0].GetInvTransform();
		matrixBlock.proj = scene->getCameras()[0].GetProjectionTransform();
		matrixBlock.viewProj = matrixBlock.proj * matrixBlock.view;
	}
}

void XRRendererTest::didUpdateRenderGraph(XRCommandBuffer* commandBuffer)
{
	commandBuffer->copyBuffer(_bufferPerFrame, 0, _uniformBufferData.size(), _uniformBufferData.data());

	{
		/*
		 *
		#define UPLOAD_METHOD_PER_DATA	0
		#define UPLOAD_METHOD_ALL_ONCE	1
		#define UPLOAD_METHOD			UPLOAD_METHOD_ALL_ONCE

		#if UPLOAD_METHOD == UPLOAD_METHOD_PER_DATA
				std::vector<void*> dataAddress;
				dataAddress.resize(programResources._indexedUniformBlockBindingInfo.size());
				dataAddress[UNIFORM_BINDING_NAME::Matrix] = &matrixBlock;
				dataAddress[UNIFORM_BINDING_NAME::Light] = &lightBlock;

				uint32_t i = 0;
				for (auto ii = programResources._indexedUniformBlockBindingInfo.begin(); ii != programResources._indexedUniformBlockBindingInfo.end(); ++ii, ++i)
				{
					if (ii->isBound() == false) continue;
					GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, ii->_offset, ii->_uniformInfo->_blockSize, dataAddress[i]));
		}
		#elif UPLOAD_METHOD == UPLOAD_METHOD_ALL_ONCE
				GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, _uniformBufferData.size(), _uniformBufferData.data()));
		#endif
		*/
	}
}

void XRRendererTest::OnRender()
{
	uint16_t frameIndex = GetRenderCounter() % 2;

	XRCommandFootprint commandFootprint;

	static xr::IndexedString<XRCommandStep> sFistStep("firstStepAlways");
	commandFootprint.AddStep(
		{ sFistStep, frameIndex },
		[this](XRCommandBuffer* secondCommands, uint16_t step) {
			static xr::IndexedString<XRWorkPassBase> renderPassSampleName = "XRWorkPassSample";
			static XRWorkPassSample* workPassSample = static_cast<XRWorkPassSample*>(_workPassManager->GetWorkPass(renderPassSampleName));

			XRBeginPassInfo beginPassInfo{
				._workPass = workPassSample
			};

			secondCommands->beginPass(beginPassInfo);

			static xr::IndexedString<XRPipeline> samplePipelineName = "SampleFirstPipeline";
			XRPipelineGroup* pipelineGroup = _pipelineManager->GetPipelineGroup(samplePipelineName);

			static uint32_t selectPermutation = [pipelineGroup]() {
				std::vector<XRPermutationElementArgument> permutation;
				permutation.emplace_back(SamplePermutation::AlphaTest, 0);
				permutation.emplace_back(SamplePermutation::Lod, 0);
				return pipelineGroup->CalcPermutationHash(permutation);
			} ();

			XRPipeline* pipeline = pipelineGroup->GetPipeline(selectPermutation);
			secondCommands->bindPipeline(XRBindPoint::Graphics, pipeline);

			//secondCommands->bindResource("", );

			XRClearValue color = xr::vec4<float_t>{ 1.f, 1.f, 1.f, 1.f };
			XRClearValue depth = XRClearValue(1.f, 0u);

			XRBeginRenderPassInfo beginRenderPassInfo{
				._renderPass = nullptr,
				._framebuffer = nullptr,
				._extent = { 1024, 768, 1 },
				._clearValues = { color, depth }
			};
			XRBeginSubPassInfo beginSubPassInfo{
				._secondaryCommandBuffer = false
			};

			secondCommands->beginRenderPass(beginRenderPassInfo, beginSubPassInfo);

			for (auto& i : _objectGroups)
			{
				XRObjectGroup const* objectGroup = i.second;
				objectGroup->draw(secondCommands);
			}

			secondCommands->endRenderPass();

			secondCommands->endPass();
		});

	static xr::IndexedString<XRCommandStep> sPresentStep("Present");
	commandFootprint.AddStep(
		{ sPresentStep, 0 },
		[this](uint16_t) {
			return uint16_t(GetAvailableBackBufferIndex());
		},
		[this](XRCommandBuffer* secondCommands, uint16_t step) {
			secondCommands->addBarrier();
		});

	std::vector<XRCommandBuffer*> commandBuffers;
	bool result = EvaluateCommands(commandFootprint, commandBuffers);
	assert(result);

	for (auto& commandBuffer : commandBuffers)
	{
		commandBuffer->executeCommands();
	}

	//commandBuffer->begin();
	//commandBuffer->end();
}
