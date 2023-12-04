#pragma once

#include <stdint.h>

#include <XRFrameworkBase/ApplicationChild.h>
#include <XRFrameworkBase/RenderEngineChild.h>

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

class XRBaseExport XRRenderEngine : public xr::ApplicationChild
{
public:
	XRRenderEngine(xr::Application* application);
	~XRRenderEngine() override;

public:
	virtual XRRenderDevice*		createRenderDevice() = 0;
	virtual XRRenderDevice*		createDefaultRenderDevice() = 0;
};

XRRenderAPI(xrCreateRenderEngine)(xr::Application* application)->XRRenderEngine*;

class XRBaseExport XRRenderDevice : public xr::RenderEngineChild
{
public:
	XRRenderDevice(XRRenderEngine* ownerRenderEngine);
	~XRRenderDevice() override;

public:
	virtual XRBuffer*			createBuffer(XRBufferCreateInfo const* createInfo) = 0;
	virtual XRTexture*			createTexture(XRTextureCreateInfo const* createInfo) = 0;
	virtual XRTexture*			createTextureFromData(XRTextureData const* loadable) = 0;
	virtual XRPipeline*			createPipeline(XRPipelineStateDescription const* description) = 0;
	virtual XRCommandBuffer*	createCommandBuffer() = 0;
	virtual XRSwapchain*		createSwapchain(XRSwapchainCreateInfo const* createInfo) = 0;
};