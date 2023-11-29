// XRRenderEngineVK.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include <XRFrameworkBase/XRRenderEngine.h>
#include <XRFrameworkBase/ApplicationChild.h>
#include <XRFrameworkBase/Application.h>

#include "XRSwapchainVK.h"

#include "XRModelVK.h"
#include "XRTextureVK.h"
#include "XRBufferVK.h"
#include "XRPipelineVK.h"
#include "XRCommandBufferVK.h"
#include "XRRenderGroupVK.h"

#include <assert.h>

class RenderEngineVulkan : public IRenderEngine
{
public:
	RenderEngineVulkan(xr::Application* application);

public:
	vk::Instance					_instance;
	std::vector<vk::PhysicalDevice>	_physicalDevices;
};

RenderEngineVulkan::RenderEngineVulkan(xr::Application* application)
	: IRenderEngine(application)
{
	const char* enableLayerNames[] = {""};
	const char* enableExtensionNames[] = {
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	};

	auto appInfo = vk::ApplicationInfo(application->getName().c_str(), 0, "XRManetEngine", 0, VK_API_VERSION_1_3);
	auto instanceInfo = vk::InstanceCreateInfo();// 0u, & appInfo, enableLayerNames, enableLayerNames);

	_instance = vk::createInstance(instanceInfo);
	_physicalDevices = _instance.enumeratePhysicalDevices();
}

XRRenderAPI(createRenderEngine)(xr::Application* application)->IRenderEngine*
{
	return new RenderEngineVulkan(application);
}

template<>
struct RenderEngineInitializer<DeviceAPI::Vulkan>
{
	RenderEngineInitializer()
	{
		LogVKSystemInfo();
	}

	static RenderEngineInitializer& GetInitializer()
	{
		static RenderEngineInitializer __init;
		return __init;
	}

	void setupStatics()
	{
		
	}

	void LogVKSystemInfo()
	{
		
	}
};

XRSwapchain* xrCreateSwapchain(XRSwapchainCreateInfo const* createInfo)
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	auto swapchainVK = new XRSwapchainVK;
	auto swapchainHandle = new XRSwapchain(createInfo, swapchainVK);

	swapchainVK->initialize(swapchainHandle);
	return swapchainHandle;
}

XRInputLayout* xrCreateInputLayout(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize)
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	return new XRInputLayoutVK(std::move(inputLayoutDesc), preferredStrideSize);
}

XRModel* xrCreateModel(XRModelData const* loadable)
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	return new XRModelVK(loadable);
}

XRTexture* xrCreateTexture(XRTextureCreateInfo const* createInfo)
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	auto textureVK = new XRTextureVK;
	auto textureHandle = new XRTexture(createInfo);
	textureHandle->_rhi = textureVK;
	return textureHandle;
}

XRTexture* xrCreateTextureFromData(XRTextureData const* loadable)
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	auto textureVK = new XRTextureVK;
	if (nullptr != loadable)
		textureVK->upload(loadable);

	auto textureHandle = new XRTexture(loadable);
	textureHandle->_rhi = textureVK;
	return textureHandle;
}

XRBuffer* xrCreateBuffer(XRBufferCreateInfo const* createInfo)
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	auto bufferVK = new XRBufferVK;
	auto bufferHandle = new XRBuffer(createInfo, bufferVK);

	bufferVK->Initialize(bufferHandle);
	return bufferHandle;
}

XRPipeline* xrCreatePipeline(XRPipelineStateDescription const* description)
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	return new XRPipelineVK(description);
}

XRCommandBuffer* xrCreateCommandBuffer()
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	return new XRCommandBufferVK;
}

XRRenderGroup* xrCreateRenderGroup()
{
	RenderEngineInitializer<DeviceAPI::Vulkan>::GetInitializer();
	return new XRRenderGroupVK;
}