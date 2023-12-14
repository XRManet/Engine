#pragma once

namespace xr
{
	class Application;
	class ApplicationPlatform;

	class XRBaseExport ApplicationChild
	{
	public:
		ApplicationChild();
		ApplicationChild(Application* ownerApplication, void(Application::* fpOnChildAdded)(ApplicationChild*), void(Application::* fpOnChildRemoved)(ApplicationChild*));
		virtual ~ApplicationChild();

	public:
		void bindApplication(Application* ownerApplication, void(Application::* fpOnChildAdded)(ApplicationChild*), void(Application::* fpOnChildRemoved)(ApplicationChild*));
		Application* getApplication() const;
		ApplicationPlatform* getApplicationPlatform() const;

	private:
		Application* _ownerApplication;
		void(Application::* _fpOnChildRemoved)(ApplicationChild*);
	};


} // namespace xr