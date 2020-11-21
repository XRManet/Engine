#include "stdafx.h"

#include "XRSceneTest.h"

#define COMPILE_CPP
#include "Shader/DeferredSample.h"

#include <XRFrameworkBase/XRObject.h>
#include <XRFrameworkBase/XRLogging.h>
#include <XRFrameworkBase/XRUtility.h>
#include <XRFrameworkBase/XRCommandBuffer.h>
#include <XRFrameworkBase/XRPipeline.h>
#include <XRFrameworkBase/XRTexture.h>

#include <XRFrameworkBase/XRActorNode.h>

#include <GL/glew.h>

#include <stdio.h>

// Todo) Menifest를 별도로 둬서 scene에서 읽을 수 있는 형식을 갖출 것
XRSceneTest::XRSceneTest()
{
	_cameras.resize(1);

	_cameras[0].SetFrustum({ 16, 9 }, 1, 10000);
	_cameras[0].SetPosition(glm::vec4{ 0, 1, 2.5, 1 });

	XRActorNode* actor = new XRActorNode();

	auto model = _resource_manager.GetModelByKey("wolf");
	glm::vec4 position{ -2, 0, 0, 1 };

	actor->BindModel(model);
	actor->SetPosition(position);

	static std::unique_ptr<XRCommandBuffer> commandBuffer(xrCreateCommandBuffer());

	//_object_groups["teapots_1"] = { commandBuffer.get(), model, std::vector<XRObject const*>(objects.begin(), objects.end()) };


}

XRSceneTest::~XRSceneTest()
{
}

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
struct PipelineKey : public PipelineKeyImpl<Permutation, Permutation::NumPermutations>
{
	PipelineKey(typename PermutationDescription<Permutation>::Type&& permutation)
	{
	}

	PipelineKey(Default) {}
};

enum SamplePermutation
{
	AlphaTest,
	Lod,
	NumPermutations
};

template<>
struct PermutationDescription<SamplePermutation>
{
	using Type = std::tuple<bool, int>;
};

class XRExport XRRenderPassSample : public XRRenderPassBase
{
	struct PipelineState;

public:
	XRRenderPassSample()
	{
		_sampleState._testInt = 5;
		//Create RenderPass
		//	Define Attachment Descriptions
		//	Define Subpasses

		//Create PipelineStateObjects
		_elementInfos.push_back({ "AlphaTest", 2 });
		_elementInfos.push_back({ "Lod", 3 });
	}

	virtual void DefaultPipelineState(XRPipelineStateDescription& outDefault) const override final
	{
		// Set Default Pipeline States
	}

	virtual bool CreatePipelineState(XRPipelineStateDescription& outPipeline) const override final
	{
		int32_t numElements = _elements.size();

		if (_elements[AlphaTest]._value == 0 && _elements[Lod]._value > 0)
		{
			return true;
		}

		return false;
	}
};

XRExport XRRenderPassSample* renderPassSample;

void XRSceneTest::Update(float dt)
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


	XRTextureCreateInfo textureInfo;
	XRTexture* textureA = xrCreateTexture(&textureInfo);

	XRCommandBuffer* commandBuffer;
	XRRenderPassBase* renderPass = renderPassSample;

	renderPass->Initialize([]() {
		});
	renderPass->Update([](SampleState& sampleState, int frameIndex) {

		});

	commandBuffer->begin();
	{
		commandBuffer->beginPass(XRPass::Graphics, renderPass);
		{

		}
		commandBuffer->endPass();
	}
	commandBuffer->end();
}



#if 0
/**********************************
 * CommandSetter Test
 */
template<typename name_tuple, typename data_tuple>
struct BaseCommandSetter
{
	BaseCommandSetter(GLuint program, name_tuple& names, const char* name, data_tuple& data)
	{
		constexpr int index = XR::util::GetIndexOfLiteralStringList<name_tuple>(names, name);
		static int bindingLocation = glGetUniformLocation(program, name);
		glUniform1f(bindingLocation, std::get<index>(data));
	}
};

template<typename name_tuple, typename data_tuple, typename var_type>
void SetVariable(GLuint program, const name_tuple names, data_tuple data, const char* name, var_type value)
{
	int index = XR::util::GetIndexOfLiteralStringList<name_tuple>(names, name);
	static int bindingLocation = glGetUniformLocation(program, name);

	std::get<index>(data) = value;
	glUniform1f(bindingLocation, value);
}

void Test()
{
	GLuint program = 9;
	glm::mat4 mvp = glm::identity<glm::mat4>();

	auto data = std::make_tuple(0.1f, 5, mvp);

	SetVariable(program, g_deferredVar, data, "var_test1", 0.2f);

	//constexpr int index = XR::util::GetIndexOfLiteralStringList(g_deferredVar, "var_test1");
	//static int bindingLocation = glGetUniformLocation(program, "var_test1");
	//glUniform1f(bindingLocation, std::get<index>(data));
}
#endif