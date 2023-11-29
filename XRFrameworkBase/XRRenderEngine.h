#pragma once

#include <stdint.h>

#include <XRFrameworkBase/ApplicationChild.h>

#include <XRFrameworkBase/XRRenderCommon.h>

#include <XRFrameworkBase/XRSwapchain.h>

#include <XRFrameworkBase/XRPipeline.h>
#include <XRFrameworkBase/XRTexture.h>
#include <XRFrameworkBase/XRBuffer.h>

enum class DeviceAPI : unsigned
{
	Null,
	OpenGL,
	Vulkan,
};

template<DeviceAPI API = DeviceAPI::Null>
struct RenderEngineInitializer
{
public:
	static RenderEngineInitializer& GetInitializer();
};

class XRBaseExport IRenderEngine : public xr::ApplicationChild
{
public:
	IRenderEngine(xr::Application* application);
	~IRenderEngine() override;
};

XRRenderAPI(createRenderEngine)(xr::Application* application)->IRenderEngine*;