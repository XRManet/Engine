#include "stdafx.h"

#include <XRFrameworkBase/Application.h>
#include <XRFrameworkBase/XRRenderEngine.h>

IRenderEngine::IRenderEngine(xr::Application* application)
	: xr::ApplicationChild(application, &xr::Application::addRenderEngine, &xr::Application::removeRenderEngine)
{
}

IRenderEngine::~IRenderEngine()
{

}
