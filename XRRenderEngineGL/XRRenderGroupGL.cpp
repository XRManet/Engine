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

/******************************************************************************
 * updateImpl
 * Description:
 *	RenderGroup의 per-vertex primitive가 새로 추가되었을 경우와 per-instance
 *	data가 새로 추가되었을 경우 등에 대한 갱신이 있어야 함.
 *	1. buffer의 공간 중 현재 사용 중인 영역은 used, 사용 중이지 않은 영역은 freed space 로 구분.
 *	2. per-vertex primitive가 추가되는 경우, 새 데이터를 vertex buffer에 추가.
 *	2-1a. 한 buffer 내에 freed space가 여럿일 수 있는데, 새로 추가될 data가 포함될 수 있는 가장 적합한 freed space를 획득
 *	2-1b. 모든 freed space가 적합하지 않은 경우 -> 'fragmentation' or 'limitation'.
 *		new buffer 생성 후 이전에 사용 중이던 old buffer의 data를 복사. -> 새 buffer의 freed space를 획득
 *	2-2. freed space에 새 데이터를 추가
 *	2-2-1. 새 model의 vertex를 read bind
 * Consideration:
 *	차후에 별도의 context 상에서 별도의 thread에서 실행될 수 있도록 염두에 두어야 함
 */

struct RenderGroupGLInfo
{
	
};

bool XRRenderGroupGL::updateImpl()
{
	auto inputLayout = getInputLayout();
	uint32_t numVertexBuffers = inputLayout->getNumVertexBuffers();

	GLintptr writeOffset = 0;
	uint32_t numObjectGroups = _objectGroups.size();
	
	for (uint32_t v = 0; v < numVertexBuffers; ++v)
	{
		glBindBuffer(GL_COPY_WRITE_BUFFER, );

		for (uint32_t i = 0; i < numObjectGroups; ++i)
		{
			auto model = static_cast<XRModelGL const*>(_objectGroups[i]->_model);
			model->getNumVertices(0, 0);

			GLintptr readOffset = 0;
			GLsizeiptr bufferSize = 0;
			// Copy vertex data from model to render group vbo with offset
			glBindBuffer(GL_COPY_READ_BUFFER, );

			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, bufferSize);
		}
	}

	return true;
}
