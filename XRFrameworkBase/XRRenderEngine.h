#pragma once

#include <stdint.h>

#include <XRFrameworkBase/ApplicationChild.h>
#include <XRFrameworkBase/RenderEngineChild.h>

class XRPipeline;			struct XRPipelineStateDescription;
class XRTexture;			struct XRTextureCreateInfo;
							class XRTextureData;
class XRBuffer;				struct XRBufferCreateInfo;
class XRSwapchain;			struct XRSwapchainCreateInfo;
class XRCommandBuffer;

enum class DeviceAPI : unsigned
{
	Null,
	OpenGL,
	Vulkan,
};

struct XRPhysicalDeviceSelectInfo
{
	uint32_t			_physicalDeviceIndex = ~0u;

	bool				_enablePresent = false;
};

struct XRQueueCreateInfo
{
	std::vector<float>	_prioritiesForGraphicsQueues;
	std::vector<float>	_prioritiesForComputeQueues;
	std::vector<float>	_prioritiesForTransferQueues;

	bool				_presentGraphics = false;
	bool				_presentCompute = false;
	bool				_presentTransfer = false;
};

struct XRRenderDeviceCreateInfo
{
	XRPhysicalDeviceSelectInfo	_physicalDeviceSelectInfo;
	XRQueueCreateInfo			_queueCreateInfo;
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
	virtual XRRenderDevice*		createRenderDevice(XRRenderDeviceCreateInfo* createInfo) = 0;
	virtual XRRenderDevice*		createDefaultRenderDevice() = 0;
};

XRRenderAPI(xrCreateRenderEngine)(xr::Application* application)->XRRenderEngine*;

class XRBaseExport XRRenderDevice : public xr::RenderEngineChild
{
public:
	XRRenderDevice(XRRenderEngine* ownerRenderEngine);
	~XRRenderDevice() override;

public:
	virtual XRRenderDeviceCreateInfo const& getRenderDeviceCreateInfo() const = 0;

public:
	virtual XRBuffer*			createBuffer(XRBufferCreateInfo const* createInfo) = 0;
	virtual XRTexture*			createTexture(XRTextureCreateInfo const* createInfo) = 0;
	virtual XRTexture*			createTextureFromData(XRTextureData const* loadable) = 0;
	virtual XRPipeline*			createPipeline(XRPipelineStateDescription const* description) = 0;
	virtual XRCommandBuffer*	createCommandBuffer() = 0;
	virtual XRSwapchain*		createSwapchain(XRSwapchainCreateInfo const* createInfo) = 0;
};