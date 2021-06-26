struct FrameConstantBuffer
{
	int		_frameCounter;
	int		_frameIndex;
	float	_deltaTime;
};

XR_RESOURCE_LAYOUT(xrSetFrameCommon, binding = 0, std140) uniform FrameConstantBuffer