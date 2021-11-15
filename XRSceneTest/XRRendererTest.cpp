#include "stdafx.h"

#include "XRRendererTest.h"

#include <XRFrameworkBase/XRUtility.h>

#include <XRFrameworkBase/XRResourceManager.h>
#include <XRFrameworkBase/XRObjectManager.h>
#include <XRFrameworkBase/XRModel.h>

#include <XRFrameworkBase/XRIndexedString.h>

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

class RenderPass(XRRenderPassPresent)
{
private:
	virtual void DefaultPipelineState(XRPipelineStateDescription & outDefault) override {}
};

class RenderPass(XRRenderPassSample)
{
private: // Common-Pipeline descriptions;
	XRInputLayout const* _inputLayout;

	friend XRRendererTest;

private: // Per-Pipeline descriptions;
	XRShaderStageDescription _defaultShaderDescription;
	XRShaderStageDescription _alphaShaderDescription;
	XRVertexInputStateDescription _defaultVertexInputStateDescription;

public:
	XRRenderPassSample()
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
	virtual void DefaultPipelineState(XRPipelineStateDescription& outDefault) override
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
	static xr::IndexedString<XRRenderPassBase> renderPassSampleName = "XRRenderPassSample";
	XRRenderPassSample* renderPassSample = static_cast<XRRenderPassSample*>(_renderPassManager->GetRenderPass(renderPassSampleName));

	_pipelineManager->CreatePipeline(renderPassSample->_pipelineCreateInfos[0]);

	XRBufferCreateInfo matrixBufferCreateInfo = {
		._size = sizeof(MatrixBlock)
	};
	_matrixBuffer = xrCreateBuffer(&matrixBufferCreateInfo);

	XRBufferCreateInfo lightBufferCreateInfo = {
		._size = sizeof(LightBlock)
	};
	_lightBuffer = xrCreateBuffer(&lightBufferCreateInfo);
}

void XRRendererTest::OnUpdate()
{
#if 0
	std::vector<unsigned char> uniformBufferData;
	{

		const size_t offsetMatrixBlock = 0;
		const size_t sizeMatrixBlock = (sizeof(MatrixBlock) * 1);
		const size_t offsetLightBlock = offsetMatrixBlock + sizeMatrixBlock;
		const size_t sizeLightBlock = (sizeof(LightBlock) * 1);
		uniformBufferData.resize(sizeMatrixBlock + sizeLightBlock);

		programResources._indexedUniformBlockBindingInfo[UNIFORM_BINDING_NAME::Matrix];
		MatrixBlock& matrixBlock = reinterpret_cast<MatrixBlock&>(uniformBufferData[offsetMatrixBlock]);
		LightBlock& lightBlock = reinterpret_cast<LightBlock&>(uniformBufferData[offsetLightBlock]);

		glm::vec3 cameraMove{
			(g_keyboardPressed['D'] - g_keyboardPressed['A']),
			(g_keyboardPressed['E'] - g_keyboardPressed['Q']),
			(g_keyboardPressed['S'] - g_keyboardPressed['W'])
		};
		glm::vec3 alignedMove = glm::mat3_cast(scene->getCameras()[0].GetQuaternion()) * cameraMove * cameraStep;
		scene->getCameras()[0].Move(alignedMove);

		static bool anchorOrientation = false;
		static glm::quat orientation;
		if (true == anchored)
		{
			static const auto up = glm::vec3(0, 1, 0);
			static const auto right = glm::vec3(1, 0, 0);

			if (false == anchorOrientation)
			{
				orientation = scene->getCameras()[0].GetQuaternion();
				anchorOrientation = true;
			}
			auto axisXangle = glm::radians(-float(curX - anchorX)) * cameraStep;
			auto axisYangle = glm::radians(float(curY - anchorY)) * cameraStep;

			auto rotation = glm::rotate(glm::rotate(orientation, axisXangle, up), axisYangle, right);
			scene->getCameras()[0].SetQuaternion(rotation);
		}
		else anchorOrientation = false;

		matrixBlock.view = scene->getCameras()[0].GetInvTransform();
		matrixBlock.proj = scene->getCameras()[0].GetProjectionTransform();
		matrixBlock.viewProj = matrixBlock.proj * matrixBlock.view;
	}

	{
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
		GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, uniformBufferData.size(), uniformBufferData.data()));
#endif
	}
#endif
}

void XRRendererTest::OnRender()
{
	uint16_t frameIndex = GetRenderCounter() % 2;

	XRCommandFootprint commandFootprint;

	static xr::IndexedString<XRCommandStep> sFistStep("firstStepAlways");
	commandFootprint.AddStep({ sFistStep, frameIndex }, [this](XRCommandBuffer* secondCommands) {
		static xr::IndexedString<XRRenderPassBase> renderPassSampleName = "XRRenderPassSample";
		static XRRenderPassSample* renderPassSample = static_cast<XRRenderPassSample*>(_renderPassManager->GetRenderPass(renderPassSampleName));

		XRRenderPassBase* renderPass = renderPassSample;
		XRBeginPassInfo beginPassInfo{
			._extent = { 1024, 768, 1 },
			._clearValues = {
				XRClearValue(1.f),
				xr::vec4<float_t>{1.f, 0.f, 0.f, 1.f},
				uint32_t(0u) },
		};

		secondCommands->beginPass(renderPass, beginPassInfo);

		static xr::IndexedString<XRPipeline> samplePipelineName = "SampleFirstPipeline";
		XRPipelineGroup* pipelineGroup = _pipelineManager->GetPipelineGroup(samplePipelineName);

		static uint32_t permutationThis = [pipelineGroup]() {
			std::vector<XRPermutationElementArgument> permutation;
			permutation.emplace_back(SamplePermutation::AlphaTest, 0);
			permutation.emplace_back(SamplePermutation::Lod, 0);
			return pipelineGroup->CalcPermutationHash(permutation);
		} ();

		XRPipeline* pipeline = pipelineGroup->GetPipeline(permutationThis);
		secondCommands->bindPipeline(XRBindPoint::Graphics, pipeline);

		//secondCommands->bindResource("", );

		for (auto& i : _objectGroups)
		{
			XRObjectGroup const* objectGroup = i.second;
			objectGroup->draw(secondCommands);
		}

		secondCommands->endPass();
		});

	static xr::IndexedString<XRCommandStep> sPresentStep("Present");
	commandFootprint.AddStep({ sPresentStep, 0 }, [this](XRCommandBuffer* secondCommands) {
		secondCommands->addBarrier();
		});

	XRCommandBuffer* commandBuffer = EvaluateCommands(commandFootprint);

	commandBuffer->executeCommands();

	//commandBuffer->begin();
	//commandBuffer->end();
}
