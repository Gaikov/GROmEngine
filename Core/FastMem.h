// Copyright (c) 2003-2007, Gaikov Roman. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FastMem.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_FastMem_H_
#define	_FastMem_H_

#include "headers.h"

template <int BLOCK_COUNT, int ITEM_SIZE, int FETCH_COUNT>
class nsFastMem
{
public:
	inline nsFastMem();
	inline void		FreeAll();
	inline void*	AllocBlock();
	inline void		FreeBlock( void* block );	//освободить блок (пометить как не используемый)
	inline void		FreeUnusedBlocks();			//освободить не используемые блоки (помеченные)
	inline void		IsEmpty() const { return m_used == 0; }

	inline void		BeginBlockFetch( int fetchNumber ) const;	//начать выборку
	inline void*	FetchBlock( int fetchNumber ) const;		//выбрать следующий блок
	inline void		EndBlockFetch( int fetchNumber ) const;		//закончить выборку

private:
	struct memBlock_t
	{
		char		data[ITEM_SIZE];	//данные блока
		bool		used;				//для страховки
		memBlock_t	*next;				//для создания списка
	};

	memBlock_t			m_data[BLOCK_COUNT];	//массив блоков
		
	memBlock_t			*m_free;				//свободные блоки
	memBlock_t			*m_used;				//используемые блоки

	mutable memBlock_t	*m_fetch[FETCH_COUNT];	//указатель для осуществления выборки
};

//---------------------------------------------------------
// nsFastMem:
//---------------------------------------------------------
template <int BLOCK_COUNT, int ITEM_SIZE, int FETCH_COUNT>
nsFastMem<BLOCK_COUNT,ITEM_SIZE,FETCH_COUNT>::nsFastMem()
{
	for ( int i = 0; i < FETCH_COUNT; i++ )
		m_fetch[i] = 0;
	m_used = 0;
	FreeAll();
}

//---------------------------------------------------------
// FreeAll:
//---------------------------------------------------------
template <int BLOCK_COUNT, int ITEM_SIZE, int FETCH_COUNT>
void nsFastMem<BLOCK_COUNT,ITEM_SIZE,FETCH_COUNT>::FreeAll()
{
	for ( int i = 0; i < FETCH_COUNT; i++ )
		m_fetch[i] = 0;
	
	m_free = 0;
	m_used = 0;
	memset( m_data, 0, sizeof(m_data) );
	
	memBlock_t	*item = m_data;
	for ( i = 0; i < BLOCK_COUNT; i++, item++ )
	{
		item->used = false;
		item->next = m_free;
		m_free = item;
	}
}

//---------------------------------------------------------
// AllocBlock:
//---------------------------------------------------------
template <int BLOCK_COUNT, int ITEM_SIZE, int FETCH_COUNT>
void* nsFastMem<BLOCK_COUNT,ITEM_SIZE,FETCH_COUNT>::AllocBlock()
{
	if ( !m_free ) return 0;
	
	memBlock_t	*item = m_free;
	m_free = m_free->next;
	
	item->used = true;
	item->next = m_used;
	m_used = item;
	
	return item;
}

//---------------------------------------------------------
// FreeBlock:
//---------------------------------------------------------
template <int BLOCK_COUNT, int ITEM_SIZE, int FETCH_COUNT>
void nsFastMem<BLOCK_COUNT,ITEM_SIZE,FETCH_COUNT>::FreeBlock( void* block )
{
	memBlock_t	*item = (memBlock_t*)block;

	if ( !m_used || !item->used ) return;
	item->used = false;
}

//---------------------------------------------------------
// FreeUnusedBlocks:
//---------------------------------------------------------
template <int BLOCK_COUNT, int ITEM_SIZE, int FETCH_COUNT>
void nsFastMem<BLOCK_COUNT,ITEM_SIZE,FETCH_COUNT>::FreeUnusedBlocks()
{
	for ( int i = 0; i < FETCH_COUNT; i++ )
	{
		assert( m_fetch[i] == 0 );	//не должна вызываться в режиме выборки
		if ( m_fetch[i] ) return;
	}
	
	memBlock_t	*item = m_used;
	memBlock_t	*prev = 0, *next = 0;
	while ( item )
	{
		if ( !item->used )
		{
			if ( !prev )
				m_used = item->next;
			else
				prev->next = item->next;
			
			next = item->next;
			item->next = m_free;
			m_free = item;
			item = next;
		}
		else
		{
			prev = item;
			item = item->next;
		}
	}
}

//---------------------------------------------------------
// BeginBlockFetch:
//---------------------------------------------------------
template <int BLOCK_COUNT, int ITEM_SIZE, int FETCH_COUNT>
void nsFastMem<BLOCK_COUNT,ITEM_SIZE,FETCH_COUNT>::BeginBlockFetch( int fetchNumber ) const
{
	assert( fetchNumber >= 0 && fetchNumber < FETCH_COUNT );
	if ( fetchNumber < 0 || fetchNumber >= FETCH_COUNT ) return;
	m_fetch[fetchNumber] = m_used;
}

//---------------------------------------------------------
// FetchBlock:
//---------------------------------------------------------
template <int BLOCK_COUNT, int ITEM_SIZE, int FETCH_COUNT>
void* nsFastMem<BLOCK_COUNT,ITEM_SIZE,FETCH_COUNT>::FetchBlock( int fetchNumber ) const
{
	assert( fetchNumber >= 0 && fetchNumber < FETCH_COUNT );
	if ( fetchNumber < 0 || fetchNumber >= FETCH_COUNT ) return 0;

	if ( !m_fetch[fetchNumber] ) return 0;
	memBlock_t	*item = m_fetch[fetchNumber];
	
	while ( item && !item->used )
		item = item->next;
	
	if ( !item )
	{
		m_fetch[fetchNumber] = 0;
		return 0;
	}
	else
		m_fetch[fetchNumber] = item->next;
	
	return item;
}

//---------------------------------------------------------
// EndBlockFetch:
//---------------------------------------------------------
template <int BLOCK_COUNT, int ITEM_SIZE, int FETCH_COUNT>
void nsFastMem<BLOCK_COUNT,ITEM_SIZE,FETCH_COUNT>::EndBlockFetch( int fetchNumber ) const
{
	assert( fetchNumber >= 0 && fetchNumber < FETCH_COUNT );
	if ( fetchNumber < 0 || fetchNumber >= FETCH_COUNT ) return;

	m_fetch[fetchNumber] = 0;
}

#endif	//_FastMem_H_

