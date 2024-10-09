#ifndef _NS_EASYSTACK_H_
#define _NS_EASYSTACK_H_

template <class T, int COUNT>
class nsEasyStack
{
public:
	nsEasyStack()
	{
		m_nSP = -1;
	}
	
	~nsEasyStack()
	{
	}

	inline bool Push( T &item )
	{
		if ( m_nSP >= COUNT - 1 ) return false;
		m_nSP ++;
		m_pstack[m_nSP] = item;
		return true;
	}

	inline bool Pop( T &item )
	{
		if ( m_nSP < 0 ) return false;
		item = m_pstack[m_nSP];
		m_nSP --;
		return true;
	}

	inline bool IsEmpty()
	{
		return m_nSP <= -1;
	}
	
	inline void Clear()
	{
		m_nSP = -1;
	}

protected:
	T	m_pstack[COUNT];
	int	m_nSP;
};

#endif
