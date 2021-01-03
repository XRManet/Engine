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

STATIC_ENUM_BEGIN(SamplePermutation, PermutationEnum);
	ADD_ENUM(SamplePermutation, AlphaTest);
	ADD_ENUM(SamplePermutation, Lod);
STATIC_ENUM_END(SamplePermutation);

template<>
struct PermutationDescription<SamplePermutation>
{
	using Type = std::tuple<bool, int>;
};

class XRExport XRRenderPassPresent : public XRRenderPassBase
{

};

class XRExport XRRenderPassSample : public XRRenderPassBase
{
private: // Common-Pipeline descriptions;
	XRInputLayout const* _inputLayout;

private: // Per-Pipeline descriptions;
	XRShaderStageDescription _defaultShaderDescription;
	XRShaderStageDescription _alphaShaderDescription;
	XRVertexInputStateDescription _defaultVertexInputStateDescription;

public:
	XRRenderPassSample(XRInputLayout const* inputLayout)
		: _inputLayout(inputLayout)
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
			
			static xr::IndexedString<XRPermutationElement> strAlphaTest = "AlphaTest";
			pipelineCreateInfo._permutationElementInfos.push_back( {strAlphaTest, 2} );
			//pipelineCreateInfo._permutationElementInfos.emplace_back( strAlphaTest, 2 );
			pipelineCreateInfo._permute = [this](XRPipelineStateDescription& outDescription, std::vector<XRPermutationElement> const& elements)
			{
				if (elements[SamplePermutation::AlphaTest]._value == 1)
				{
					outDescription._shaderStageDescription = &_alphaShaderDescription;
				}

				return false;
			};
		}
	}
};

XRExport XRRenderPassSample* renderPassSample;

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
	XRShaderStageDescription _defaultShaderDescription;
	XRShaderStageDescription _alphaShaderDescription;
	XRVertexInputStateDescription _defaultVertexInputStateDescription;
	
	static_assert(SamplePermutation::AlphaTest == 0, "");
	printf("%s %d\n", SamplePermutation::AlphaTest->_id, SamplePermutation::AlphaTest);
	printf("%d %s\n", SamplePermutation::AlphaTest, SamplePermutation::AlphaTest->_id);
	
	SamplePermutation a = SamplePermutation::AlphaTest;
	printf("%d %s\n", a, a->_id);
	
	a = SamplePermutation::Lod;
	printf("%d %s\n", a, a->_id);
	
	a = 0;
	printf("%d %s\n", a, a->_id);

	{
		XRPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo._name = xr::IndexedString<XRPipeline>("SampleFirstPipeline");

		const char* CommonVertex = "SimpleVertex.glsl";
		_defaultShaderDescription._vertexFilename = CommonVertex;
		_defaultShaderDescription._fragmentFilename = "SimpleFragment.glsl";
		_alphaShaderDescription._vertexFilename = CommonVertex;
		_alphaShaderDescription._fragmentFilename = "SimpleFragmentAlpha.glsl";
		pipelineCreateInfo._description._shaderStageDescription = &_defaultShaderDescription;

		auto testInputLayout = resourceManager->GetInputLayoutByCategory("Skeletal");
		_defaultVertexInputStateDescription.init(testInputLayout, XRPrimitiveTopology::TriangleList);
		pipelineCreateInfo._description._vertexInputStateDescription = &_defaultVertexInputStateDescription;

		pipelineCreateInfo._permutationElementInfos.push_back({ "AlphaTest", 2 });
		pipelineCreateInfo._permute = [&_alphaShaderDescription](XRPipelineStateDescription& outDescription, std::vector<XRPermutationElement> const& elements)
		{
			if (elements[SamplePermutation::AlphaTest]._value == 1)
			{
				outDescription._shaderStageDescription = &_alphaShaderDescription;
			}

			return true;
		};

		_pipelineManager->CreatePipeline(std::move(pipelineCreateInfo));
	}
}

void XRRendererTest::Update()
{
}

void XRRendererTest::Render()
{
	int frameIndex = 0;

	renderPassSample->Initialize([]() {
		printf("initialize test\n");
		});

	renderPassSample->Update([](SampleState& sampleState, int frameIndex) {
		printf("update test\n");

		PipelineKey<SamplePermutation>* pipelineKey = nullptr;
		if (frameIndex % 2 == 0)
		{
			static PipelineKey<SamplePermutation> pipelineKeyPermuted(std::make_tuple(true, 1));
			pipelineKey = &pipelineKeyPermuted;
		}
		else
		{
			static PipelineKey<SamplePermutation> pipelineKeyPermuted(std::make_tuple(true, 0));
			pipelineKey = &pipelineKeyPermuted;
		}

		//

		});

	CommandFootprint commandFootprint;

	static xr::IndexedString<CommandStep> sFistStep("firstStepAlways");
	commandFootprint.push_back({ sFistStep, 0 }, [this](XRCommandBuffer* secondCommands) {
		XRRenderPassBase* renderPass = renderPassSample;
		XRFramebuffer* frameBuffer;
		xr::vec3<uint32_t> extent { 1024, 768, 1 };

		secondCommands->beginPass(renderPass, extent);

		std::vector<XRPermutationElementArgument> permutation;
		XRPipeline* pipeline = _pipelineManager->GetPipeline("samplePipelineName", permutation);
		secondCommands->bindPipeline(XRBindPoint::Graphics, pipeline);

		secondCommands->endPass();
		});

	XRTextureCreateInfo textureInfo;
	XRTexture* textureA = xrCreateTexture(&textureInfo);

	XRCommandBuffer* commandBuffer;
	XRRenderPassBase* renderPass = renderPassSample;
	XRFramebuffer* framebuffer;

	commandBuffer->begin();
	commandBuffer->end();
}
