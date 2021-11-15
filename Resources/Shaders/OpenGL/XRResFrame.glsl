layout(XR_SET(xrSetFrameCommon) binding = 0, std140) uniform FrameConstantBuffer
{
	int		_frameCounter;
	int		_frameIndex;
	float	_deltaTime;
};