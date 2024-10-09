#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "nsLib/comm_types.h"
#include <cassert>
#include <cstddef>
#include <cstring>

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
//	������ �� ������ ����������/������������� 
//  � �������������/������������ ���������� �������
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 

void*	mem_malloc( uint size, const char *file, int line );
void*	mem_realloc( void *data, uint size, const char *file, int line );
void	mem_free( void *data );
void	mem_report( uint &userAlloc, uint &heapAlloc );
char*	mem_strdup( const char *str, const char *file, int line );
void    mem_report();

#define my_malloc( size ) mem_malloc( size, __FILE__, __LINE__ )
#define my_realloc( data, size ) mem_realloc( data, size, __FILE__, __LINE__ )
#define my_free( data ) mem_free( data )
#define my_strdup( str ) mem_strdup( str, __FILE__, __LINE__ )

template <class T>
inline void MemZero( T &mem )
{
	memset( &mem, 0, sizeof(T) );
}

void* operator new ( size_t size );
void* operator new[] ( size_t size );
void operator delete( void* p ) noexcept;
void operator delete [] ( void* p ) noexcept;


/*inline void* operator new ( size_t size, const char* file, int line )
{
	return mem_malloc( size, (char*)file, line );
}

inline void* operator new[] ( size_t size, const char* file, int line )
{
	return mem_malloc( size, (char*)file, line );
}

inline void operator delete( void* p )
{
	mem_free( p );
}

inline void operator delete [] ( void* p )
{
	mem_free( p );
}

#define new new( __FILE__, __LINE__ )//*/

//#pragma warning( disable : 4291 )
#endif
