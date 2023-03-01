// XRRenderEngineVK.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "XRModelVK.h"
#include "XRTextureVK.h"
#include "XRBufferVK.h"
#include "XRPipelineVK.h"
#include "XRCommandBufferVK.h"
#include "XRRenderGroupVK.h"

#include <assert.h>

struct RenderEngineVKInitializer
{
	RenderEngineVKInitializer()
	{
		LogVKSystemInfo();
	}

	static RenderEngineVKInitializer& GetInitializer()
	{
		static RenderEngineVKInitializer __init;
		return __init;
	}

	void LogVKSystemInfo()
	{
		
	}
};

XRInputLayout* xrCreateInputLayout(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize)
{
	RenderEngineVKInitializer::GetInitializer();
	return new XRInputLayoutVK(std::move(inputLayoutDesc), preferredStrideSize);
}

XRModel* xrCreateModel(XRModelData const* loadable)
{
	RenderEngineVKInitializer::GetInitializer();
	return new XRModelVK(loadable);
}

XRTexture* xrCreateTexture(XRTextureCreateInfo const* createInfo)
{
	RenderEngineVKInitializer::GetInitializer();
	auto textureVK = new XRTextureVK;
	auto textureHandle = new XRTexture(createInfo);
	textureHandle->_rhi = textureVK;
	return textureHandle;
}

XRTexture* xrCreateTextureFromData(XRTextureData const* loadable)
{
	RenderEngineVKInitializer::GetInitializer();
	auto textureVK = new XRTextureVK;
	if (nullptr != loadable)
		textureVK->upload(loadable);

	auto textureHandle = new XRTexture(loadable);
	textureHandle->_rhi = textureVK;
	return textureHandle;
}

XRBuffer* xrCreateBuffer(XRBufferCreateInfo const* createInfo)
{
	RenderEngineVKInitializer::GetInitializer();
	auto bufferVK = new XRBufferVK;
	auto bufferHandle = new XRBuffer(createInfo, bufferVK);

	bufferVK->Initialize(bufferHandle);
	return bufferHandle;
}

XRPipeline* xrCreatePipeline(XRPipelineStateDescription const* description)
{
	RenderEngineVKInitializer::GetInitializer();
	return new XRPipelineVK(description);
}

XRCommandBuffer* xrCreateCommandBuffer()
{
	RenderEngineVKInitializer::GetInitializer();
	return new XRCommandBufferVK;
}

XRRenderGroup* xrCreateRenderGroup()
{
	RenderEngineVKInitializer::GetInitializer();
	return new XRRenderGroupVK;
}
