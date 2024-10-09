//
// Created by Roman Gaikov on 2019-09-09.
//

#include "Lock.h"

void nsLock::AddLock(const char *id)
{
	if (!HasLock(id))
	{
		_locks.emplace_back(id);
	}
}

void nsLock::RemoveLock(const char *id)
{
	auto it = std::find(_locks.begin(), _locks.end(), id);
	if (it != _locks.end())
	{
		_locks.erase(it);
	}
}

bool nsLock::IsLocked() const
{
	return !_locks.empty();
}

bool nsLock::HasLock(const char *id) const
{
	auto it = std::find(_locks.begin(), _locks.end(), id);
	return it != _locks.end();
}
