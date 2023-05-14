#include "stdafx.h"
#include "ApplicationChild.h"
#include "Application.h"

namespace xr
{

	ApplicationChild::ApplicationChild()
		: _ownerApplication(nullptr)
		, _fpOnChildRemoved(nullptr)
	{

	}

	ApplicationChild::ApplicationChild(Application* ownerApplication, void(Application::* fpOnChildAdded)(ApplicationChild*), void(Application::* fpOnChildRemoved)(ApplicationChild*))
		: _ownerApplication(ownerApplication)
		, _fpOnChildRemoved(fpOnChildRemoved)
	{
		assert(fpOnChildAdded != nullptr);
		assert(fpOnChildRemoved != nullptr);

		(ownerApplication->*fpOnChildAdded)(this);
	}

	ApplicationChild::~ApplicationChild()
	{
		if (_ownerApplication)
		{
			(_ownerApplication->*_fpOnChildRemoved)(this);
		}
	}

	void ApplicationChild::bindApplication(Application* ownerApplication, void(Application::* fpOnChildAdded)(ApplicationChild*), void(Application::* fpOnChildRemoved)(ApplicationChild*))
	{
		_ownerApplication = ownerApplication;
		_fpOnChildRemoved = fpOnChildRemoved;

		assert(fpOnChildAdded != nullptr);
		assert(fpOnChildRemoved != nullptr);

		(ownerApplication->*fpOnChildAdded)(this);
	}

	ApplicationPlatform* ApplicationChild::getApplicationPlatform() const
	{
		assert(_ownerApplication != nullptr);
		return _ownerApplication->getApplicationPlatform();
	}

} // namespace xr