#include "stdafx.h"

#include "XRIndexedString.h"

#include <XRFrameworkBase/XRPlatform.h>

class XRPipeline;

namespace xr
{
// Note(jiman): 특정 타입에 대한 템플릿 인스턴싱이 수행될 때마다 static member의 정의도 존재해야 함.
//				더불어 이 정의는 프로세스 내에 공유해서 사용되어야 하므로 import 해야함.
//				이 때 모든 모듈에서 이 기호의 정의를 확인할 수 있도록 어떤 모듈에선 정의된 기호를 export 해야함.
#if defined(XR_BUILD_SYSTEM_MSBUILD_VS)
#if XR_BUILD_SYSTEM_MSBUILD_VS_2019
#define EXPLICIT_GEN_INDEXED_STRING_SYMBOL(Category, ...) \
	XRBaseExport IndexedStringContainer<Category, __VA_ARGS__> IndexedString<Category, __VA_ARGS__>::sContainer_PS;
#elif XR_BUILD_SYSTEM_MSBUILD_VS_2017
#define EXPLICIT_GEN_INDEXED_STRING_SYMBOL(Category, ...) \
	XRBaseExport IndexedStringContainer<Category, __VA_ARGS__> IndexedString<Category, __VA_ARGS__>::sContainer_PS;
//	template<> XRBaseExport IndexedStringContainer<Category, __VA_ARGS__> IndexedString<Category, __VA_ARGS__>::sContainer_PS;
#endif // XR_BUILD_SYSTEM_MSBUILD_VS

#elif defined(XR_BUILD_SYSTEM_XCODE)
#define EXPLICIT_GEN_INDEXED_STRING_SYMBOL(Category, ...) \
	static_assert(false, "Not implemented yet");
#endif // defined(XR_BUILD_SYSTEM)

// Note(jiman): XRFrameworkBase 에서 정의한 IndexedString에 대해 프로세스 전체에 걸쳐 전역적인 관리 객체 하나에만 접근되어야 한다.
//				이를 위해선 XRFrameworkBase 외의 어떤 프로젝트에서 처음 사용하는 구체적 타입(Specific Instance)에 대해서도
//				링크 타임에 연결할 수 있는 심볼이 필요하다.
//				이 cpp 파일을 개별 프로젝트 모두가 각자 컴파일하게 만들면 프로젝트 영역 내에서만 한정적인 관리 객체가 생성되므로
//				프로젝트 영역을 넘나드는 접근이 불가능해진다.
//				당장은 우선 사용이 필요한 타입에 대한 실타입을 필요할 때마다 이쪽에서 직접 명시적으로 생성하도록 하되,
//				pre-link event 내에서 가능한 방법을 모색해볼 것. (이런 용도의 심볼을 공급할 수 있는 추가 프로젝트를 생성하는 것도 고려)
// Debugging:	XRFrameworkBase.dll!xr::IndexedString<Category,unsigned short>::sContainer
EXPLICIT_GEN_INDEXED_STRING_SYMBOL(DefaultStringCategory);
EXPLICIT_GEN_INDEXED_STRING_SYMBOL(XRPipeline);
}