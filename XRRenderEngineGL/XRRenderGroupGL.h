//
//  XRRenderGroupGL.h
//  XRRenderEngineGL
//
//  Created by Jiman Jeong on 2020/10/01.
//  Copyright © 2020 XRManet. All rights reserved.
//

#pragma once

#include <XRFrameworkBase/XRRenderGroup.h>

class XRInputLayoutGL;

class XRRenderGroupGL :	public XRRenderGroup
{
private:
	GLuint _vertexBuffers[16];
	uint32_t _numVertexBuffers;

protected:
	inline XRInputLayoutGL const* getInputLayout() const;
	
private:
	bool updateImpl() override final;
};

