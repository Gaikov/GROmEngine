#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "nsLib/comm_types.h"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <new>

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
//	������ �� ������ ����������/������������� 
//  � �������������/������������ ���������� �������
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 

void*	mem_malloc( size_t size, const char *file, int line );
void*	mem_realloc( void *data, size_t size, const char *file, int line );
void	mem_free( void *data );
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
void* operator new ( size_t size, std::align_val_t alignment );
void* operator new[] ( size_t size, std::align_val_t alignment );
void* operator new ( size_t size, const std::nothrow_t& ) noexcept;
void* operator new[] ( size_t size, const std::nothrow_t& ) noexcept;
void* operator new ( size_t size, std::align_val_t alignment, const std::nothrow_t& ) noexcept;
void* operator new[] ( size_t size, std::align_val_t alignment, const std::nothrow_t& ) noexcept;

void operator delete( void* p ) noexcept;
void operator delete [] ( void* p ) noexcept;
void operator delete( void* p, size_t size ) noexcept;
void operator delete [] ( void* p, size_t size ) noexcept;
void operator delete( void* p, std::align_val_t alignment ) noexcept;
void operator delete [] ( void* p, std::align_val_t alignment ) noexcept;
void operator delete( void* p, size_t size, std::align_val_t alignment ) noexcept;
void operator delete [] ( void* p, size_t size, std::align_val_t alignment ) noexcept;
void operator delete( void* p, const std::nothrow_t& ) noexcept;
void operator delete [] ( void* p, const std::nothrow_t& ) noexcept;
void operator delete( void* p, std::align_val_t alignment, const std::nothrow_t& ) noexcept;
void operator delete [] ( void* p, std::align_val_t alignment, const std::nothrow_t& ) noexcept;


class nsMemory final {
public:
	static void StartTracking();
	static void StopTracking();
	static void BeginLoop();
	static void EndLoop();
	static void PushLoopAllocationsScope();
	static void PopLoopAllocationsScope();
	static int LiveAllocations();
};

class nsMemoryLoopAllocScope final {
public:
	nsMemoryLoopAllocScope() { nsMemory::PushLoopAllocationsScope(); }
	~nsMemoryLoopAllocScope() { nsMemory::PopLoopAllocationsScope(); }
};

#endif
