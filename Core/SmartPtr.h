// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SmartPtr.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_SmartPtr_H_
#define	_SmartPtr_H_

template <class T>
class nsSmartPtr
{
public:
    nsSmartPtr() :
            m_object(nullptr),
            m_refCount(nullptr) {
    }

    nsSmartPtr(T *obj) :
            m_object(obj),
            m_refCount(nullptr) {
        if (obj)
            m_refCount = new int(1);
    }
	  
	nsSmartPtr( const nsSmartPtr<T> &other )
	{
		m_object = other.m_object;
		m_refCount = other.m_refCount;
		if ( m_refCount ) (*m_refCount)++;
	}
	  
	virtual ~nsSmartPtr()
	{
		DecRef();
	}
	  
	nsSmartPtr& operator = ( const nsSmartPtr<T>& other )
	{
		if ( this == &other ) return *this;
		  
		DecRef();
		m_object = other.m_object;
		m_refCount = other.m_refCount;
		if ( m_refCount ) (*m_refCount)++;
		  
		return *this;
	}

    bool operator == (const nsSmartPtr<T> &other) {
        return m_object == other.m_object;
    }
	  
	T* operator-> () const
	{
		return m_object;
	}
	
	operator T* () const
	{
		return m_object;
	}
	  
private:
	T	*m_object;
	int	*m_refCount;
	
private:
	void	DecRef()
	{
		if ( m_object )
		{
			(*m_refCount)--;
			if ( (*m_refCount) <= 0 )
			{
				delete m_refCount;
				delete m_object;
				m_refCount = 0;
				m_object = 0;
			}
		}
	}
};

#endif //_SmartPtr_H_