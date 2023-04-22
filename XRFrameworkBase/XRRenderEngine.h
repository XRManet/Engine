#pragma once

#include <stdint.h>

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

enum XRLoopManagerInfra : unsigned;
template<XRLoopManagerInfra Infra>
class XRWindowSystem;

template<DeviceAPI API = DeviceAPI::Null>
struct RenderEngineInitializer
{
public:
	static RenderEngineInitializer& GetInitializer();
	
public:
	template<XRLoopManagerInfra Infra>
	void initialize(XRWindowSystem<Infra> const& infra);
};
