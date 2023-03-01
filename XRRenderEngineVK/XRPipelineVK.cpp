#include "stdafx.h"
#include "XRPipelineVK.h"

#include <XRFrameworkBase/XRSourceBuildSystem.h>
#include <glm/vec4.hpp>

namespace
{
// Move to a project 'XRSourceBuildSystemGLSL'
}

XRPipelineVK::XRPipelineVK(XRPipelineStateDescription const* description)
	: XRPipeline(description)
{
	XRSourceBuildSystem* glslBuildSystem = xrGetShaderBuildSystem();
	XRCompiler* glslCompiler = glslBuildSystem->getCompiler();

	XRBuildConfiguration buildConfiguration = {};
	XRBuildItemManifest buildItemManifest = {};
	buildItemManifest._shaderStageDescription = description->_shaderStageDescription;

	//XRGLProgram* glProgram = static_cast<XRGLProgram*>(glslCompiler->BuildExecutable(&buildConfiguration, &buildItemManifest));
	//if (glProgram == nullptr)
	//	throw;

	_pipeline = VK_NULL_HANDLE; //glProgram->_program;

	bool result = createBindingInformation();
	if (result == false)
		throw;
}

XRPipelineVK::~XRPipelineVK()
{
}

void XRPipelineVK::bind()
{
}

struct ProgramResourceVK : public ProgramResources
{
};


bool XRPipelineVK::createBindingInformation()
{
	return true;
}

namespace
{
// Move to a project 'XRSourceBuildSystemGLSL'
#define DEFAULT_RESOURCE_PATH "Resources/Shaders/Vulkan/"
constexpr size_t RESOURCE_PATH_END = sizeof(DEFAULT_RESOURCE_PATH) - 1;

}