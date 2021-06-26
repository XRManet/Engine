#include "stdafx.h"

char const* GetGlErrorString(GLenum errorCode)
{
	static char stringifiedErrorCode[32] = { 0, };

	switch (errorCode)
	{
	case GL_INVALID_ENUM:           return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:          return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:      return "GL_INVALID_OPERATION";
	default:
		sprintf_s(stringifiedErrorCode, "0x%x", errorCode);
		return stringifiedErrorCode;
	}
	return nullptr;
}
