//
// Created by Roman Gaikov on 2020-01-31.
//

#ifndef __SUBSYSTEM_H__
#define __SUBSYSTEM_H__

#include "headers.h"

template<typename T>
class nsSubSystem
{
public:
	static bool Init();
	static void Release();
	static T *Shared();

    nsSubSystem(const nsSubSystem &) = delete;
    nsSubSystem &operator=(const nsSubSystem &) = delete;

protected:
	nsSubSystem() = default;
	virtual ~nsSubSystem() = default;

	virtual bool OnInit() {return true;}
	virtual void OnRelease() {}

protected:
	static T *_instance;
};

template<typename T> T *nsSubSystem<T>::_instance = nullptr;

template<typename T>
bool nsSubSystem<T>::Init()
{
	//assert(_instance == nullptr);
	if (!_instance)
	{
		_instance = new T();
		return dynamic_cast<nsSubSystem<T>*>(_instance)->OnInit();
	}
	return true;
}

template<typename T>
void nsSubSystem<T>::Release()
{
	assert(_instance != nullptr);
	if (_instance)
	{
		dynamic_cast<nsSubSystem<T>*>(_instance)->OnRelease();
		delete _instance;
		_instance = nullptr;
	}
}

template<typename T>
T *nsSubSystem<T>::Shared()
{
    if (!_instance) {
	    assert(_instance != nullptr);
    }
	return _instance;
}

#endif //__SUBSYSTEM_H__
