//
//  XRRenderGroupGL.h
//  XRRenderEngineGL
//
//  Created by Jiman Jeong on 2020/10/01.
//  Copyright © 2020 XRManet. All rights reserved.
//

#pragma once

#include <XRFrameworkBase/XRRenderGroup.h>

class XRRenderGroupGL : public XRRenderGroup
{
public:
	XRRenderGroupGL() = default;
	
public:
	bool updateImpl() override final;
};
