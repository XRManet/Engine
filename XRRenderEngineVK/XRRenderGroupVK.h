//
//  XRRenderGroupVK.h
//  XRRenderEngineVK
//
//  Created by Jiman Jeong on 2020/10/01.
//  Copyright © 2020 XRManet. All rights reserved.
//

#pragma once

#include <XRFrameworkBase/XRRenderGroup.h>

class XRInputLayoutVK;
class XRBufferView;

class XRRenderGroupVK :	public XRRenderGroup
{
private:
	XRBufferView* _vertexBuffers[16];
	uint32_t _numVertexBuffers;

protected:
	inline XRInputLayoutVK const* getInputLayout() const;
	
private:
	bool updateImpl() override final;
};

