#pragma once

#include <XRFrameworkBase/XRDefault.h>

#include <memory.h>
#include <unordered_map>
#include <vector>
#include <string>

#include <XRFrameworkBase/XRLock.h>

namespace xr
{
struct DefaultStringCategory {};

template<typename Category, typename Capacity = uint16_t>
struct XRBaseExport IndexedStringContainer
{
	IndexedStringContainer()
		: _allStrings({ {"", 0 } })
		, _allAccessors({ &_allStrings.begin()->first })
		, _indexCounter(1)
	{
	}

public:
	std::unordered_map<std::string, Capacity> _allStrings;
	std::vector<std::string const*> _allAccessors;
	Capacity _indexCounter;
	MutexLock _mutexLock;
};

template<typename Category = DefaultStringCategory, typename Capacity = uint16_t>
class XRBaseExport IndexedString
{
public:
	IndexedString()
		: _index(0)
	{
	}

	IndexedString(IndexedString const& rhs)
		: _index(rhs._index)
	{
	}

	IndexedString(IndexedString&& rhs)
		: _index(rhs._index)
	{
		rhs._index = 0;
	}

	IndexedString& operator = (IndexedString const& rhs)
	{
		_index = rhs._index;
		return *this;
	}

	IndexedString& operator = (IndexedString&& rhs)
	{
		_index = rhs._index;
		rhs._index = 0;
		return *this;
	}

	IndexedString(char const* string)
		: IndexedString(std::string(string))
	{
	}

#define IndexedStringLiteral(string, index) { #string, index }
	constexpr IndexedString(char const* string, Capacity index) : _index(index)
	{

	}

	IndexedString(std::string const& string)
	{
		MutexLock& mutexLock = sContainer._mutexLock;
		ScopedLocker<MutexLock> locker(mutexLock);
		auto found = sContainer._allStrings.find(string);
		if (found != sContainer._allStrings.end())
			_index = found->second;
		else
		{
			_index = sContainer._indexCounter++;
			auto result = sContainer._allStrings.insert({ string, _index });
			if (result.second == true)
			{
				sContainer._allAccessors.push_back(&(result.first->first));
			}
		}
	}

public:
	inline bool operator == (IndexedString const& rhs) const { return (_index == rhs._index); }
	inline bool operator != (IndexedString const& rhs) const { return (_index != rhs._index); }

	friend std::hash<xr::IndexedString<Category, Capacity>>;

public:
	inline char const* c_str() const { return sContainer._allAccessors[_index]->c_str(); }
	inline std::string const& string() const { return *sContainer._allAccessors[_index]; }
	inline size_t length() const { return sContainer._allAccessors[_index]->length(); }
	inline Capacity const getIndex() const { return _index; }

private:
	Capacity _index = 0;

private:
	//XRBaseExport	static IndexedStringContainer<Category, Capacity>	sContainer_PS;
	//				static IndexedStringContainer<Category, Capacity>&	sContainer;
	/*XRBaseExport*/	static IndexedStringContainer<Category, Capacity>	sContainer;
#define sContainer_PS sContainer
};

//template<typename Category, typename Capacity>
//IndexedStringContainer<Category, Capacity>& IndexedString<Category, Capacity>::sContainer = IndexedString<Category, Capacity>::sContainer_PS;
}

/**
 * @struct	std::hash<xr::IndexedString<Category,Capacity>>
 *
 * @brief	An hash for xr::IndexedString<Category,Capacity>. std::hash<Capacity> 와 동일
 *
 * @author	Jiman Jeong
 * @date	2020-11-28
 *
 * @tparam	Category	Type of the category.
 * @tparam	Capacity	Type of the capacity.
 */

template<typename Category, typename Capacity>
struct std::hash<xr::IndexedString<Category, Capacity>>
{
	inline size_t operator () (xr::IndexedString<Category, Capacity> const& indexedString) const
	{
		return std::hash<Capacity>()(indexedString._index);
	}
};

#include <XRFrameworkBase/XRPlatform.h>

namespace xr
{
// Note(jiman): 특정 타입에 대한 템플릿 인스턴싱이 수행될 때마다 static member의 정의도 존재해야 함.
//				더불어 이 정의는 프로세스 내에 공유해서 사용되어야 하므로 import 해야함.
//				이 때 모든 모듈에서 이 기호의 정의를 확인할 수 있도록 어떤 모듈에선 정의된 기호를 export 해야함.
#if XR_BUILD_SYSTEM == XR_BUILD_SYSTEM_MSBUILD_VS
#if XR_BUILD_SYSTEM_MSBUILD_VS_2019
#define EXPLICIT_GEN_INDEXED_STRING_SYMBOL(Category, ...) \
	template IndexedString<Category, ##__VA_ARGS__>; \
	XRBaseExport IndexedStringContainer<Category, ##__VA_ARGS__> IndexedString<Category, ##__VA_ARGS__>::sContainer_PS;
#elif XR_BUILD_SYSTEM_MSBUILD_VS_2017
#define EXPLICIT_GEN_INDEXED_STRING_SYMBOL(Category, ...) \
	template IndexedString<Category, ##__VA_ARGS__>; \
	XRBaseExport IndexedStringContainer<Category, ##__VA_ARGS__> IndexedString<Category, ##__VA_ARGS__>::sContainer_PS;
	//template<> XRBaseExport IndexedStringContainer<Category, __VA_ARGS__> IndexedString<Category, __VA_ARGS__>::sContainer_PS;
#endif // XR_BUILD_SYSTEM_MSBUILD_VS

#elif XR_BUILD_SYSTEM == XR_BUILD_SYSTEM_XCODE
#if XR_BUILD_SYSTEM_XCODE_CLANG_11
// Note(jiman): template class member를 export할 땐 instantiation을 통해 해당 심볼의 storage를 반드시 특정 라이브러리에 한정시켜야 함.
//				clang은 VC++과 달리 class member에 대해 implicit instantiation을 사용할 수 없는 것으로 보임.
//				때문에 explicit instantiation으로 처리하되, 이 때 이 static member에 대한 template definition이 필요하므로 이 cpp 내에 한정적으로 정의함.
template<typename Category, typename Capacity>
IndexedStringContainer<Category, Capacity> IndexedString<Category, Capacity>::sContainer_PS;

#define EXPLICIT_GEN_INDEXED_STRING_SYMBOL(Category, ...) \
	template IndexedString<Category, ##__VA_ARGS__>; \
	template XRBaseExport IndexedStringContainer<Category, ##__VA_ARGS__> IndexedString<Category, ##__VA_ARGS__>::sContainer_PS;
#else
#define EXPLICIT_GEN_INDEXED_STRING_SYMBOL(Category, ...) \
	static_assert(false, "Not implemented yet");
#endif
#endif // defined(XR_BUILD_SYSTEM)
}
