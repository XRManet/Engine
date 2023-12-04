#include <XRFrameworkBase/XRPlatform.h>

#include <XRFrameworkBase/XRModel.h>
#include <XRFrameworkBase/XRTexture.h>
#include <XRFrameworkBase/XRBuffer.h>
#include <XRFrameworkBase/XRPipeline.h>
#include <XRFrameworkBase/XRCommandBuffer.h>
#include <XRFrameworkBase/XRRenderGroup.h>
#include <XRFrameworkBase/XRSwapchain.h>

namespace xr
{
class Application;
}

class XRRenderEngine;
class XRRenderDevice;

XRRenderAPI(xrCreateRenderEngine)(xr::Application* application)->XRRenderEngine* = nullptr;

XRRenderAPI(xrCreateInputLayout)(XRInputLayoutDesc&& inputLayoutDesc, uint32_t preferredStrideSize)->XRInputLayout* = nullptr;
XRRenderAPI(xrCreateModel)(XRModelData const* loadable)->XRModel* = nullptr;
XRRenderAPI(xrCreateRenderGroup)()->XRRenderGroup* = nullptr;

XRRenderAPI(xrCreateTexture)(XRRenderDevice* ownerRenderDevice, XRTextureCreateInfo const* createInfo)->XRTexture* = nullptr;
XRRenderAPI(xrCreateTextureFromData)(XRRenderDevice* ownerRenderDevice, XRTextureData const* loadable)->XRTexture* = nullptr;

XRRenderAPI(xrCreateBuffer)(XRRenderDevice* ownerRenderDevice, XRBufferCreateInfo const* createInfo)->XRBuffer* = nullptr;

XRRenderAPI(xrCreatePipeline)(XRRenderDevice* ownerRenderDevice, XRPipelineStateDescription const* createInfo)->XRPipeline* = nullptr;

XRRenderAPI(xrCreateCommandBuffer)(XRRenderDevice* ownerRenderDevice)->XRCommandBuffer* = nullptr;

XRRenderAPI(xrCreateSwapchain)(XRRenderDevice* ownerRenderDevice, XRSwapchainCreateInfo const* createInfo)->XRSwapchain* = nullptr;

#define XRRENDER_ENGINE_GL			XR_DYNAMIC_LIBRARY(XRRenderEngineGL)
#define XRRENDER_ENGINE_VK			XR_DYNAMIC_LIBRARY(XRRenderEngineVK)
#define XRRENDER_ENGINE_DEFAULT     XRRENDER_ENGINE_GL

const char* g_xrRenderEngineNameDso = nullptr;

static struct XRRenderEngineLinker
{
	XRPlatform::XRDSO* _dso;

	XRRenderEngineLinker()
		: _dso(nullptr)
	{
		_dso = XRPlatform::LoadDSO(g_xrRenderEngineNameDso);
		assert(_dso != nullptr);

		// Todo) List up and check availability
		std::vector<std::string> dllLists;
		XRPlatform::ListDLLFunctions(_dso, dllLists);

		GetProcAddress(xrCreateRenderEngine,	"xrCreateRenderEngine");
		GetProcAddress(xrCreateInputLayout,		"xrCreateInputLayout");
		GetProcAddress(xrCreateModel,			"xrCreateModel");
		GetProcAddress(xrCreateRenderGroup,		"xrCreateRenderGroup");
		GetProcAddress(xrCreateTexture,			"xrCreateTexture");
		GetProcAddress(xrCreateTextureFromData,	"xrCreateTextureFromData");
		GetProcAddress(xrCreateBuffer,			"xrCreateBuffer");
		GetProcAddress(xrCreatePipeline,		"xrCreatePipeline");
		GetProcAddress(xrCreateCommandBuffer,	"xrCreateCommandBuffer");
		GetProcAddress(xrCreateSwapchain,		"xrCreateSwapchain");
	}

	~XRRenderEngineLinker()
	{
		XRPlatform::UnloadDSO(_dso);
	}

private:
	template<typename func>
	void GetProcAddress(func& outFunction, const char* name)
	{
		outFunction = reinterpret_cast<func>(XRPlatform::GetProcAddress(_dso, name));
		assert(outFunction != nullptr);
	}
} EngineLinkerStarter;
