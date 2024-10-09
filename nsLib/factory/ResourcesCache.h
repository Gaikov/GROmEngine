//
// Created by Roman on 30/07/2019.
//
#pragma once
#include "nsLib/headers.h"

template<class TResource, class TParam>
class ResourcesCache
{
public:
	ResourcesCache() = default;
	void ReleaseAll();
	TResource* GetResource(const char *resourceName, TParam param);
	void ReleaseResource(TResource *item, bool force = false);

protected:
	virtual TResource* AllocateResource(const char *resourceName, TParam param) = 0;
	virtual void FreeResource(TResource *item) = 0;
	void AddToCache(const char *resourceName, TResource *item);

protected:
	struct Entry
	{
		TResource *item;
		int refCount;
	};

	std::map<std::string, Entry> _cache;
};

template<class TResource, class TParam>
void ResourcesCache<TResource, TParam>::AddToCache(const char *resourceName, TResource *item)
{
	assert(_cache.find(resourceName) == _cache.end());
	Entry newEntry;
	newEntry.refCount = 1;
	newEntry.item = item;

	_cache.insert(std::make_pair(resourceName, newEntry));
}

template<class TItem, class TParam>
TItem* ResourcesCache<TItem, TParam>::GetResource(const char *resourceName, TParam param)
{
	if (!resourceName)
	{
		return nullptr;
	}

	auto it = _cache.find(resourceName);
	if (it != _cache.end())
	{
		Entry &e = it->second;
		e.refCount ++;
		return e.item;
	}

	auto item = AllocateResource(resourceName, param);
	if (item)
	{
		AddToCache(resourceName, item);
		return item;
	}

	return nullptr;
}

template<class TItem, class TParam>
void ResourcesCache<TItem, TParam>::ReleaseResource(TItem *item, bool force)
{
	for (auto it : _cache)
	{
		Entry &entry = it.second;
		if (entry.item == item)
		{
			entry.refCount --;
			if (entry.refCount <= 0 || force)
			{
				_cache.erase(it.first);
				FreeResource(entry.item);
				return;
			}
		}
	}
}

template<class TItem, class TParam>
void ResourcesCache<TItem, TParam>::ReleaseAll()
{
	for (auto it : _cache)
	{
		Entry &entry = it.second;
		FreeResource(entry.item);
	}
	_cache.clear();
}