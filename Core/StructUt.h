// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file StructUt.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_StructUt_H_
#define	_StructUt_H_

#include "Memory.h"

//-----------------------------------------------------
//  AddToArray:  
//-----------------------------------------------------
template<class T>
inline bool AddToArray_( T** array, int& count, const T& data, const char* file, int line )
{
	if ( !*array )
	{
		count = 0;
		//*array = (T*)my_malloc( sizeof(T) );
		*array = (T*)mem_malloc( sizeof(T), file, line );
	}
	else
		//*array = (T*)my_realloc( *array, sizeof(T) * (count + 1) );
		*array = (T*)mem_realloc( *array, sizeof(T) * (count + 1), file, line );
	
	if ( !*array ) return false;
	
	memcpy( (*array) + count, &data, sizeof(T) );
	count++;
	return true;
}

#define AddToArray( array, count, data ) \
	AddToArray_( array, count, data, __FILE__, __LINE__ )

//-----------------------------------------------------
//  AddToList:  
//-----------------------------------------------------
template <class T>
inline void AddToList( T** lst, T* data )
{
	if ( *lst ) (*lst)->prev = data;
	data->next = *lst;
	(*lst) = data;
}


//-----------------------------------------------------
//  DelFromList:  
//-----------------------------------------------------
template <class T>
inline void DelFromList( T** lst, T* data )
{
	if ( *lst == data ) *lst = data->next;
	T* prev = data->prev;
	T* next = data->next;
	if ( prev ) prev->next = next;
	if ( next ) next->prev = prev;
}

#endif	//_StructUt_H_