// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// OpenGLStart.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.

char const* GetGlErrorString(GLenum errorCode)
{
    static char stringifiedErrorCode[32] = {0, };
    
    switch(errorCode)
    {
        case GL_INVALID_ENUM:           return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:          return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:      return "GL_INVALID_OPERATION";
        default:
            sprintf(stringifiedErrorCode, "0x%x", errorCode);
            return stringifiedErrorCode;
    }
    return nullptr;
}
