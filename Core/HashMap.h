#ifndef _HASH_MAP_H_
#define _HASH_MAP_H_

#include "nsLib/StrTools.h"
#include "StructUt.h"
#include "Memory.h"

//-----------------------------------------------------
//  class nsHashMap:  
//-----------------------------------------------------
template <class T>
class nsHashMap
{
public:
	nsHashMap();

	bool	SetData( const char* key, T* data );	//false - если такой ключ уже есть
	T*		GetData( const char* key ) const;		//NULL - not found
	
	bool	DeleteKey( const char* key );
	bool	DeleteAllKeys();	//нужно удалить данные перед удалением ключа/ей

	//последовательная обработка (выборка) элементов
	bool	FetchBegin() const;
	T*		FetchNext() const;
	void	FetchEnd() const;

private:
	struct hashItem_t
	{
		T*		userData;
		char*		key;
		hashItem_t	*prev, *next;
	};

	hashItem_t	*m_hash[256];
	
	mutable bool		m_fetch;
	mutable int			m_fetchList;
	mutable hashItem_t	*m_fetchItem;

private:
	nsHashMap( const nsHashMap& hashMap ) {}
	nsHashMap& operator = ( const nsHashMap& hashMap ) { return *this; }

	hashItem_t* FindItem( const char* key ) const;
};

//////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------
//  nsHashMap::nsHashMap:  
//-----------------------------------------------------
template <class T>
nsHashMap<T>::nsHashMap()
{
	memset( m_hash, 0, sizeof(m_hash) );
	m_fetch = false;
}

//-----------------------------------------------------
//  nsHashMap::SetData:  
//-----------------------------------------------------
template <class T>
inline bool nsHashMap<T>::SetData( const char* key, T* data )
{
	if ( GetData( key ) ) return false;
	
	hashItem_t	*item = (hashItem_t*)my_malloc( sizeof(hashItem_t) );
	item->key = my_strdup( key );
	item->userData = data;
	item->prev = item->next = 0;

	AddToList( &m_hash[ StrHash( key ) ], item );
	return true;
}

//-----------------------------------------------------
//  nsHashMap::GetData:  
//-----------------------------------------------------
template <class T>
inline T* nsHashMap<T>::GetData( const char* key ) const
{
	hashItem_t	*item = FindItem( key );
	if ( item ) return item->userData;
	return 0;
}

//-----------------------------------------------------
//  nsHashMap::DeleteKey:  
//-----------------------------------------------------
template <class T>
inline bool nsHashMap<T>::DeleteKey( const char* key )
{
	assert( !m_fetch );	//нельзя удалять ключ в режиме выборки
	hashItem_t	*item = FindItem( key );
	if ( !item ) return false;
	
	DelFromList( &m_hash[ StrHash( key ) ], item );
	my_free( item->key );
	my_free( item );
	return true;
}

//-----------------------------------------------------
//  nsHashMap::DeleteAllKeys:  
//-----------------------------------------------------
template <class T>
bool nsHashMap<T>::DeleteAllKeys()
{
	assert( !m_fetch );	//нельзя удалять ключи в режиме выборки

	for ( int i = 0; i < 256; i++ )
	{
		hashItem_t	*item = m_hash[i];
		while ( item )
		{
			hashItem_t	*del = item;
			item = item->next;
			my_free( del->key );
			my_free( del );
		}
	}

	memset( m_hash, 0, sizeof(m_hash) );
	return true;
}

//-----------------------------------------------------
//  nsHashMap<T>::FetchBegin:  
//-----------------------------------------------------
template <class T>
bool nsHashMap<T>::FetchBegin() const
{
	m_fetch = true;
	m_fetchList = 0;
	m_fetchItem = 0;
	return true;
}

//-----------------------------------------------------
//  nsHashMap<T>::FetchNext:  
//-----------------------------------------------------
template <class T>
inline T* nsHashMap<T>::FetchNext() const
{
	assert( m_fetch );	//перед вызовом FetchNext нужно вызвать FetchBegin

	if ( m_fetchItem )
		m_fetchItem = m_fetchItem->next;
	
	while ( m_fetchList < 256 && !m_fetchItem )
	{
		m_fetchItem = m_hash[ m_fetchList ];
		m_fetchList ++;
	}

	if ( !m_fetchItem ) return 0;
	return m_fetchItem->userData;
}

//-----------------------------------------------------
//  nsHashMap<T>::FetchEnd:
//-----------------------------------------------------
template <class T>
inline void nsHashMap<T>::FetchEnd() const
{
	m_fetch = false;
	m_fetchItem = 0;
}

//-----------------------------------------------------
//  nsHashMap::FindItem:  
//-----------------------------------------------------
template <class T>
inline typename nsHashMap<T>::hashItem_t* nsHashMap<T>::FindItem( const char* key ) const
{
	for ( hashItem_t *item = m_hash[StrHash( key )]; item; item = item->next )
	{
		if ( StrEqual( key, item->key ) )
			return item;
	}
	return 0;
}

#endif