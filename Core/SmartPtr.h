// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SmartPtr.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

template <class T>
class nsSmartPtr final
{
public:
    nsSmartPtr() = default;

    nsSmartPtr(T *obj) {
        if (obj) {
	        _ref = new ObjectRef;
        	_ref->_object = obj;
        	_ref->_refCount = 1;
        }
    }
	  
	nsSmartPtr( const nsSmartPtr<T> &other )
	{
    	_ref = other._ref;
    	IncRef();
	}
	  
	~nsSmartPtr()
	{
		DecRef();
	}
	  
	nsSmartPtr& operator = ( const nsSmartPtr<T>& other )
	{
		if ( this == &other ) return *this;
		  
		DecRef();
    	_ref = other._ref;
    	IncRef();

		return *this;
	}

	// move constructor
	nsSmartPtr(nsSmartPtr<T>&& other) noexcept {
    	_ref = other._ref;
    	other._ref = nullptr;
    }

	// move assignment
	nsSmartPtr<T>& operator=(nsSmartPtr<T>&& other) noexcept {
    	if (this == &other) return *this;
    	DecRef();
    	_ref = other._ref;
    	other._ref = nullptr;
    	return *this;
    }

    bool operator == (const nsSmartPtr<T> &other) const {
        return other._ref == _ref;
    }

	bool operator != (const nsSmartPtr<T> &other) const {
    	return other._ref != _ref;
    }
	  
	T* operator-> () const
	{
		return _ref ? _ref->_object : nullptr;
	}
	
	operator T* () const
	{
		return _ref ? _ref->_object : nullptr;
	}
	  
private:
	struct ObjectRef {
		int	_refCount = 0;
		T	*_object = nullptr;
	};

	ObjectRef *_ref = nullptr;

	void DecRef()
	{
		if ( _ref ) {
			--_ref->_refCount;
			if ( _ref->_refCount <= 0 ) {
				delete _ref->_object;
				delete _ref;
				_ref = nullptr;
			}
		}
	}

	void IncRef() {
		if ( _ref ) {
			++_ref->_refCount;
		}
	}
};