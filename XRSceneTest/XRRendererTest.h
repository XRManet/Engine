#pragma once

#include <XRFrameworkBase/XRRenderer.h>
#include <XRFrameworkBase/XRRenderEngine.h>

class XRRendererTest : public XRRenderer
{
	// Todo) 일단 렌더링 되는지 테스트해보려고 여기다 선언해서 그냥 써봄.
	// 아래 멤버변수들은 그래픽스 파이프라인에서나 선언해다 쓰고
	// 나중엔 Rendering stratagy와 pipeline 간의 관계를 정립한 뒤,
	// 렌더링으로 연결될 수 있게 변경할 것.
	XRPipeline* _pipeline = nullptr;

	struct UNIFORM_BINDING_NAME
	{
		enum : uint32_t
		{
			Matrix,
			Light,
			Material,
			Count
		};
	};

	std::vector<unsigned char> _uniformBufferData;

	XRBuffer* _matrixBuffer;
	XRBuffer* _lightBuffer;

	struct Light
	{
		float position[4];
		float intensity[3];
		float attenuation;
	};

	struct Material
	{
		float diffuse[4];
	};

public:
	XRRendererTest();
	~XRRendererTest();

	void Initialize(XRResourceManager* resourceManager) override final;

	void WillUpdateRenderGraph(XRScene* scene) override final;
	void didUpdateRenderGraph(XRCommandBuffer* commandBuffer) override final;
	void OnRender() override final;
};

extern "C" XRExport XRRenderer * CreateRendererTest()
{
	return new XRRendererTest;
}