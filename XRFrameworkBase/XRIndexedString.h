#pragma once

#include "stdafx.h"

#include <memory.h>
#include <unordered_map>
#include <vector>
#include <string>

#include <XRFrameworkBase/XRLock.h>

namespace xr
{
struct DefaultStringCategory {};

template<typename Category, typename Capacity = uint16_t>
struct IndexedStringContainer
{
	IndexedStringContainer();

public:
	std::unordered_map<std::string, Capacity> _allStrings;
	std::vector<std::string const*> _allAccessors;
	Capacity _indexCounter;
	MutexLock _mutexLock;
};

template<typename Category, typename Capacity>
inline IndexedStringContainer<Category, Capacity>::IndexedStringContainer()
	: _allStrings({ {"", 0 } })
	, _allAccessors({ &_allStrings.begin()->first })
	, _indexCounter(1)
{
}

template<typename Category = DefaultStringCategory, typename Capacity = uint16_t>
class IndexedString
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

	IndexedString(std::string const& string)
	{
		ScopedLocker<MutexLock> locker(sContainer._mutexLock);
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
	inline size_t length() const { return sContainer._allAccessors[_index]->length(); }

private:
	Capacity _index = 0;

private:
	XRBaseExport	static IndexedStringContainer<Category, Capacity>	sContainer_PS;
					static IndexedStringContainer<Category, Capacity>&	sContainer;
};

template<typename Category, typename Capacity>
IndexedStringContainer<Category, Capacity>& IndexedString<Category, Capacity>::sContainer = IndexedString<Category, Capacity>::sContainer_PS;
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
