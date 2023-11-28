// main.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <XREntryPoint/MainApplication.h>

int main(int argc, char** argv)
{
	std::unique_ptr<xr::Application> app(new xr::MainApplication(xr::PlatformType::Win32));
	xr::Application::runApplication(app.get(), argc, argv);

	return 0;
}