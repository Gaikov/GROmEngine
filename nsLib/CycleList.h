// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file CycleList.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_CycleList_H_
#define	_CycleList_H_

template <class T, int count>
class nsCycleList
{
public:
	nsCycleList()
	{
		m_count = count;
		m_max = 0;
		m_curr = 0;
		m_add = 0;
	}

	void Add( T &item )
	{
		m_buff[m_add] = item;
		m_max ++;

		if ( ++m_add >= m_count )
		{
			m_add = 0;
			m_max = m_count;
		}

		m_curr = m_add;
	}

	T& GetPrev()
	{
		if ( m_max < m_count )
		{
			if ( --m_curr < 0 )
			{
				if ( m_max ) m_curr = m_max - 1;
				else m_curr = 0;
			}
		}
		else
		{
			if ( --m_curr < 0 ) m_curr = m_count - 1;
		}
		
		return m_buff[m_curr];
	}

	T& GetCurr()
	{
		if ( m_max < m_count )
		{
			if ( ++m_curr >= m_max ) m_curr = 0;
		}
		else
		{
			if ( ++m_curr >= m_count ) m_curr = 0;
		}

		//log_printf( "get curr: %i\n", m_curr );
		return m_buff[m_curr];
	}

private:
	int		m_min, m_max;
	int		m_curr;
	int		m_count;
	int		m_add;

	T		m_buff[count];
};

#endif //_CycleList_H_