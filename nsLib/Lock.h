//
// Created by Roman Gaikov on 2019-09-09.
//

#ifndef __LOCK_H__
#define __LOCK_H__

#include "headers.h"
#include "StrTools.h"

class nsLock
{
public:
	void AddLock(const char *id);
	void RemoveLock(const char *id);
	bool HasLock(const char *id) const;
	bool IsLocked() const;

private:
	std::vector<nsString>   _locks;
};


#endif //__LOCK_H__
