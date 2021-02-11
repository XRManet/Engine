#include "stdafx.h"

#include "XRRendererTest.h"

#include <XRFrameworkBase/XRUtility.h>

#include <XRFrameworkBase/XRResourceManager.h>
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

XRRenderPassSample* renderPassSample;

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
	renderPassSample = static_cast<XRRenderPassSample*>(_renderPassManager->GetRenderPass(renderPassSampleName));

	_pipelineManager->CreatePipeline(renderPassSample->_pipelineCreateInfos[0]);
}

void XRRendererTest::OnUpdate()
{
}

void XRRendererTest::OnRender()
{
	int frameIndex = GetRenderCounter() % 2;

	XRCommandFootprint commandFootprint;

	static xr::IndexedString<XRCommandStep> sFistStep("firstStepAlways");
	commandFootprint.AddStep({ sFistStep, frameIndex }, [this](XRCommandBuffer* secondCommands) {
		XRRenderPassBase* renderPass = renderPassSample;
		XRFramebuffer* frameBuffer;
		xr::vec3<uint32_t> extent { 1024, 768, 1 };

		secondCommands->beginPass(renderPass, extent);

		static xr::IndexedString<XRPipeline> samplePipelineName = "samplePipelineName";
		XRPipelineGroup* pipelineGroup = _pipelineManager->GetPipelineGroup(samplePipelineName);

		static uint32_t permutationThis = [pipelineGroup]() {
			std::vector<XRPermutationElementArgument> permutation;
			permutation.emplace_back(SamplePermutation::AlphaTest, 0);
			permutation.emplace_back(SamplePermutation::Lod, 0);
			return pipelineGroup->CalcPermutationHash(permutation);
		} ();

		XRPipeline* pipeline = pipelineGroup->GetPipeline(permutationThis);
		secondCommands->bindPipeline(XRBindPoint::Graphics, pipeline);

		XRObjectGroup const* teapotGroup = scene->getObjectGroup("teapots_1");

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
