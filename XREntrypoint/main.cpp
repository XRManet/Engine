// OpenGLStart.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <XRFrameworkBase/XRSourceBuildSystem.h>
#include <XRFrameworkBase/XRRenderEngine.h>
#include "XRLoopManager.h"
#include "XRSceneManager.h"

#define XRInfraAPI_GLFW		XRLoopManagerInfra::GLFW
#define XRInfraAPI			XRInfraAPI_GLFW


#include "XRRenderEnginePlatformBinder.hpp"

int main(void)
{
	XRLoopManager<XRInfraAPI> loopManager({ 800.f, 600.f });
	

	XRSourceBuildSystem* shaderBuildSystem = xrGetShaderBuildSystem();

	auto sceneManager = XRSceneManager::GetInstance();

	sceneManager->BindPrimaryScene("main");

	loopManager.Initialize();

	loopManager.Run();

	return 0;
}