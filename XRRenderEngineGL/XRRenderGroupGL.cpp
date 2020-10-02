//
//  XRRenderGroupGL.cpp
//  XRRenderEngineGL
//
//  Created by Jiman Jeong on 2020/10/01.
//  Copyright © 2020 XRManet. All rights reserved.
//

#include "stdafx.h"

#include "XRRenderGroupGL.h"
#include "XRModelGL.h"

#include <XRFrameworkBase/XRObjectManager.h>

/*=============================================================================
 * Inline, static functions
 */

XRInputLayoutGL const* XRRenderGroupGL::getInputLayout() const
{
	return static_cast<XRInputLayoutGL const*>(XRRenderGroup::getInputLayout());
}

/*=============================================================================
 * Implementations
 */

bool XRRenderGroupGL::updateImpl()
{
	auto inputLayout = getInputLayout();

	uint32_t numObjectGroups = _objectGroups.size();
	for (uint32_t i = 0; i < numObjectGroups; ++i)
	{
		auto model = static_cast<XRModelGL const*>(_objectGroups[i]->_model);
		
		// Copy vertex data from model to render group vbo with offset

		glCopyBufferSubData()
	}

	return true;
}
