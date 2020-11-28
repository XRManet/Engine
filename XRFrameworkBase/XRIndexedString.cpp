#include "stdafx.h"

#include "XRIndexedString.h"

class XRPipeline;

namespace xr
{
template<typename Category, typename Capacity>
std::unordered_map<std::string, Capacity> IndexedString<Category, Capacity>::sAllStrings = { { "", 0 } };

template<typename Category, typename Capacity>
std::vector<std::string*> IndexedString<Category, Capacity>::sAllAccessors;

template<typename Category, typename Capacity>
Capacity IndexedString<Category, Capacity>::sIndexCounter = 1;

template<typename Category, typename Capacity>
MutexLock IndexedString<Category, Capacity>::sMutexLock;

// Note(jiman): XRFrameworkBase 에서 정의한 IndexedString에 대해 프로세스 전체에 걸쳐 전역적인 관리 객체 하나에만 접근되어야 한다.
//				이를 위해선 XRFrameworkBase 외의 어떤 프로젝트에서 처음 사용하는 구체적 타입(Specific Instance)에 대해서도
//				링크 타임에 연결할 수 있는 심볼이 필요하다.
//				이 cpp 파일을 개별 프로젝트 모두가 각자 컴파일하게 만들면 프로젝트 영역 내에서만 한정적인 관리 객체가 생성되므로
//				프로젝트 영역을 넘나드는 접근이 불가능해진다.
//				당장은 우선 사용이 필요한 타입에 대한 실타입을 필요할 때마다 이쪽에서 직접 명시적으로 생성하도록 하되,
//				pre-link event 내에서 가능한 방법을 모색해볼 것. (이런 용도의 심볼을 공급할 수 있는 추가 프로젝트를 생성하는 것도 고려)
#define EXPLICIT_GEN_INDEXED_STRING_SYMBOL(Category, ...) \
	template<> XRBaseExport std::unordered_map<std::string, uint16_t> IndexedString<Category, __VA_ARGS__>::sAllStrings = { { "", 0 } }; \
	template<> XRBaseExport std::vector<std::string*> IndexedString<Category, __VA_ARGS__>::sAllAccessors; \
	template<> XRBaseExport uint16_t IndexedString<Category, __VA_ARGS__>::sIndexCounter = 1; \
	template<> XRBaseExport MutexLock IndexedString<Category, __VA_ARGS__>::sMutexLock;

EXPLICIT_GEN_INDEXED_STRING_SYMBOL(DefaultStringCategory);
EXPLICIT_GEN_INDEXED_STRING_SYMBOL(XRPipeline);
}