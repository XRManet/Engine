#pragma once

#include <stdint.h>

#include <XRFrameworkBase/XRRenderCommon.h>
#include <XRFrameworkBase/XRPipeline.h>
#include <XRFrameworkBase/XRTexture.h>
#include <XRFrameworkBase/XRBuffer.h>

enum DeviceAPI : unsigned
{
	Null,
	OpenGL,
	Vulkan,
};

enum XRLoopManagerInfra : unsigned;
template<XRLoopManagerInfra Infra>
class XRRenderingInfra;

template<DeviceAPI API = DeviceAPI::Null>
struct RenderEngineInitializer
{
public:
	static RenderEngineInitializer& GetInitializer();
	
public:
	template<XRLoopManagerInfra Infra>
	void initialize(XRRenderingInfra<Infra> const& infra);
};
