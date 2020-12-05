#pragma once

#include <XRFrameworkBase/XRRenderer.h>
#include <XRFrameworkBase/XRRenderEngine.h>

class XRRendererTest : public XRRenderer
{
	// Todo) �ϴ� ������ �Ǵ��� �׽�Ʈ�غ����� ����� �����ؼ� �׳� �ẽ.
	// �Ʒ� ����������� �׷��Ƚ� ���������ο����� �����ش� ����
	// ���߿� Rendering stratagy�� pipeline ���� ���踦 ������ ��,
	// ���������� ����� �� �ְ� ������ ��.
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

	std::vector<XRBuffer*> _uniformBuffers;

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

	void Initialize() override;

	void Update() override;
	void Render() override;
};

extern "C" XRExport XRRenderer * CreateRendererTest()
{
	return new XRRendererTest;
}