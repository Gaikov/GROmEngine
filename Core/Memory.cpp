#include "Memory.h"
#include "sys.h"
#include "nsLib/log.h"
#include "nsLib/StrTools.h"
#include <cstdlib>

void *operator new(size_t size)
{
	return mem_malloc(size, "unknown file", 0);
}

void *operator new[](size_t size)
{
	return mem_malloc(size, "unknown file", 0);
}

void operator delete(void *p) noexcept
{
	mem_free(p);
}

void operator delete[](void *p) noexcept
{
	mem_free(p);
}

#define    MEM_ID    0xAFAF

struct memblock_t
{
	uword id{};
	uint  real_size{};    //real size with header
	uint  alloc_size{}; //user size, when call malloc/realloc

	nsString file;
	int      line{};

	memblock_t *next{}, *prev{};
};


static memblock_t *g_memList  = nullptr;
static uint       g_nHeapSize = 0;
static uint       g_nUserSize = 0;

#ifdef _TRACK_MEMORY_

//---------------------------------------------------------
// mem_malloc: 
//---------------------------------------------------------
void* mem_malloc( uint size, const char *file, int line )
{
	uint		real_size = size + sizeof(memblock_t);
	void		*data = malloc( real_size );
	auto *block = (memblock_t*)data;
	if ( !block )
	{
		Sys_FatalError( "ERROR: Allocate memory block! (file: [%s], line: [%i])", file, line );
		return nullptr;
	}
	memset( block, 0, real_size );
	
	//fill header
	block->id = MEM_ID;
	block->file = file;
	block->line = line;
	block->real_size = real_size;
	block->alloc_size = size;
	g_nHeapSize += block->real_size;
	g_nUserSize += block->alloc_size;
	
	//past to queue
	block->next = g_memList;
	if ( g_memList ) g_memList->prev = block;
	g_memList = block;

	return (block + 1);
}

//---------------------------------------------------------
// mem_realloc: 
//---------------------------------------------------------
void* mem_realloc( void *data, uint size, const char *file, int line )
{
	memblock_t	*old_block = ((memblock_t*)data) - 1;
	uint		real_size = size + sizeof(memblock_t);
	auto *block = (memblock_t*)::malloc( real_size );
	if ( !block )
	{
		Sys_FatalError( "ERROR: Allocate memory block! (file: [%s], line: [%i])", file, line );
		return nullptr;
	}
	memset( block, 0, real_size );
	
	//fill header
	block->id = MEM_ID;
	block->file = file;
	block->line = line;
	block->real_size = real_size;
	block->alloc_size = size;
	g_nHeapSize += block->real_size;
	g_nUserSize += block->alloc_size;
	
	//past to queue
	block->next = g_memList;
	if ( g_memList ) g_memList->prev = block;
	g_memList = block;

	//copy data
	int	copy_size = size > old_block->alloc_size ? old_block->alloc_size : size;
	memcpy( block + 1, old_block + 1, copy_size );
	mem_free( old_block + 1 ); //free old block 

	return (block + 1);
}

//---------------------------------------------------------
// mem_free: 
//---------------------------------------------------------
void mem_free( void *data )
{
	if ( !data ) return;

	//LogPrintf( PRN_ALL, "free block: %x\n", data );
	memblock_t	*block = ((memblock_t*)data) - 1;
	if ( !g_memList || block->id != MEM_ID )
	{
		LogPrintf( PRN_DEV, "WARNING: free invalid allocated block\n" );
		return;
	}

	if ( block == g_memList )
		g_memList = block->next;
	else
	{
		memblock_t	*prev = block->prev;
		memblock_t	*next = block->next;
		//LogPrintf( PRN_ALL, "free OK\n" );
		if ( prev ) prev->next = next;
		if ( next ) next->prev = prev;
	}

	g_nHeapSize -= block->real_size;
	g_nUserSize -= block->alloc_size;
	free( block );
}

#else

static int g_allocatedBlocks = 0;

void *mem_malloc(uint size, const char *file, int line)
{
    g_allocatedBlocks ++;
	auto data = malloc(size);
	memset(data, 0, size);
	return data;
}

void *mem_realloc(void *data, uint size, const char *file, int line)
{
	assert(data);
	return realloc(data, size);
}

void mem_free(void *data)
{
	if (data)
	{
        g_allocatedBlocks --;
		free(data);
	}
}

#endif

//---------------------------------------------------------
// my_strdup:
//---------------------------------------------------------
char *mem_strdup(const char *str, const char *file, int line)
{
	if (!str)
	{
		assert(!"Invalid param!");
		return nullptr;
	}

	auto *res = (char *) mem_malloc(strlen(str) + 1, file, line);
	strcpy(res, str);
	return res;
}

//-----------------------------------------------------
// mem_report:
//-----------------------------------------------------
void mem_report(uint &userAlloc, uint &heapAlloc)
{
	userAlloc = g_nUserSize;
	heapAlloc = g_nHeapSize;
}

void mem_report()
{
	if (g_memList)
	{
		FILE *fp = fopen("mem_leaks.txt", "w");
		if (fp)
		{
			fprintf(fp, "memory allocated: %i\n", g_nHeapSize);
			memblock_t *block = g_memList;
			bool       found  = block != nullptr;
			while (block)
			{
				fprintf(fp, "file: [%s], line: [%i], size: %i (%i)\n", block->file.AsChar(), block->line,
				        block->alloc_size, block->real_size);
				block = block->next;
			}

			fclose(fp);
		}

		Sys_Message("Memory leaks detected!");
	}

    if (g_allocatedBlocks != 0) {
        printf("WARNING: Leaked mem blocks: %i\n", g_allocatedBlocks);
    }
}



