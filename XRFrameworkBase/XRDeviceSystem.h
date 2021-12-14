#pragma once

struct XRDeviceSystemCompatibility
{

};

class XRDeviceSystemAttachment
{
public:
	virtual ~XRDeviceSystemAttachment() = default;
};

/**
 * @class	XRDeviceSystem
 *
 * @brief	RenderEngine 혹은 ComputeEngine 같은 별도의 API set으로 구성된 체계의 interface
 *
 * @author	Jiman Jeong
 * @date	2021-02-21
 */

class XRDeviceSystem
{
public:
	XRDeviceSystem() = default;

public:
	virtual bool AttachDeviceSystemAttachment(XRDeviceSystemAttachment* attachment) = 0;
};