/************************************************ 
EXERCISE      : Variable Size Allocator
Implmented by : Snir Holland
Reviewed by   : Asaf Talker  
Date          : 24/5/2023
File          : vsa.c
*************************************************/

#include <assert.h> /* assert */
#include <limits.h> /* LONG_MAX */

#include "vsa.h"

#define GETMAX(x,y)     (x > y) ? (x) : (y)
#define WORD_LEN        (sizeof(size_t))
#define HEADER_SIZE     (sizeof(vsa_t))
#define INVALID         (0)
#define SIGNATURE       (0xDEADBEEF)

/* Helper Function: aligns a memory block (rounding up) */
static size_t AlignBlock(size_t block_size);

/* Helper Function: jump to the next marker */
static vsa_t *GetNextMarker(vsa_t *vsa);

/* Helper Function: update size contained in marker */ 
static void UpdateMarkerSize(vsa_t *vsa, long size);

/* Helper Function: add DEADBEEF signature */ 
static void AddSignature(vsa_t *vsa);

/* Helper Function: defrag memory blocks */ 
static vsa_t *Defrag(vsa_t *vsa , size_t nbytes);

/* Helper Function: init markers */ 
static void InitMarkers(vsa_t *start, vsa_t *end, size_t pool_size);


struct header
{
	long size;
	#ifndef NDEBUG 
	long pool_id; 
	#endif 
}; 

/*--------------VSAInit--------------------*/
vsa_t *VSAInit(void *memory, size_t pool_size)
{
	vsa_t *start = NULL;
	vsa_t *end   = NULL;
	
	assert(NULL != memory);
	assert(pool_size > 0);
	
	pool_size = AlignBlock(pool_size);
	
	/* starting marker of the pool */
	start = (vsa_t *)memory;
	
	/* init first and last markers of pool */
	InitMarkers(start, end, pool_size);
	
	return (vsa_t*)start;
} 

/*--------------VSAAlloc--------------------*/
void *VSAAlloc(vsa_t *vsa, size_t nbytes)
{
	vsa_t *new_marker = NULL;
	
	assert(NULL != vsa);
	assert(nbytes > 0);

	nbytes = AlignBlock(nbytes);
	
	/* locate header for allocation */
	vsa = Defrag(vsa, nbytes);
	if(NULL == vsa)
	{
		return NULL;
	}

	/* allocate new block and update its marker */
	if(nbytes + HEADER_SIZE < (size_t)(vsa->size))
	{
		new_marker = ((vsa_t *)((char *)vsa + nbytes + HEADER_SIZE));
		
		UpdateMarkerSize(new_marker, vsa->size - ((long)nbytes + HEADER_SIZE));
		AddSignature(new_marker);
	}
	
	/* update previous marker */
	UpdateMarkerSize(vsa, -((long)nbytes + HEADER_SIZE)); 

	return (char *)vsa + HEADER_SIZE;
}

/*--------------VSAFree--------------------*/
void VSAFree(void *block)
{
	vsa_t *header = NULL;	
	
	/* if starting address of block points to NULL, do nothing */
	if (NULL == block)
	{
		return;
	}
	
	/* arrive to the header that the memory block belongs to */
	header = (vsa_t *)((char *)block - HEADER_SIZE);
	
	/* make sure that the memory block to be freed is ours */		
	#ifndef NDEBUG 
	assert(SIGNATURE == header->pool_id);
	#endif
	
	/* mark bytes as free by making the header size positive */
	UpdateMarkerSize(header, -(header->size));  
}

/*--------------VSALargestChunkAvailabe--------------------*/
size_t VSALargestChunkAvailabe(vsa_t *vsa)
{
	vsa_t *runner = NULL;
	
	long largest_chunk = 0;
	
	assert(NULL != vsa);
	
	/* perform defrag on the entire memory pool */
	Defrag(vsa, LONG_MAX);
	
	/* scan for chunk with maximum size*/
	runner = vsa;
	while (INVALID != runner->size)
	{
		largest_chunk = GETMAX(largest_chunk , runner->size);
		runner = GetNextMarker(runner);
	}
	
	return (largest_chunk - HEADER_SIZE);
}

/* Helper Function: absolute value (from long to size_t) */
static size_t Abs(long num)
{
	return (num > 0 ? num : -num);
}

/* Helper Function: aligns a memory block (rounding up) */
static size_t AlignBlock(size_t block_size)
{
	if (0 != block_size % WORD_LEN)
	{
		block_size = (block_size / WORD_LEN + 1) * WORD_LEN;		 
	}
	
	return block_size;
}

/* Helper Function: jump to the next marker */
static vsa_t *GetNextMarker(vsa_t *vsa)
{
	return (vsa_t *)((char *)vsa + Abs(vsa->size));
}

/* Helper Function: update size contained in marker */ 
static void UpdateMarkerSize(vsa_t *vsa, long new_size)
{
	vsa->size = new_size;
}

/* Helper Function: add DEADBEEF signature */ 
static void AddSignature(vsa_t *vsa)
{
	#ifndef NDEBUG 
	vsa->pool_id = SIGNATURE; 
	#endif 
}

/* Helper Function: init markers */ 
static void InitMarkers(vsa_t *start, vsa_t *end, size_t pool_size)
{
	UpdateMarkerSize(start, pool_size - HEADER_SIZE);
	AddSignature(start);
	
	end = GetNextMarker(start);
	
	UpdateMarkerSize(end, INVALID);
	AddSignature(end);
}

/* Helper Function: defrag memory blocks */ 
static vsa_t *Defrag(vsa_t *vsa , size_t nbytes)
{
	vsa_t *runner_back = vsa;
	vsa_t *runner_front = NULL;
	
	/* continue until we have enough bytes or reach the end */
	while ((INVALID != runner_back->size) && (runner_back->size <= (long)nbytes))
	{
		/* free bytes exists, but still not enough for allocation*/
		if (runner_back->size > 0)
		{
			/* defragmentation of memory blocks */
			runner_front = GetNextMarker(runner_back);
			while (runner_front->size > 0)
			{
				UpdateMarkerSize(runner_back, runner_back->size + runner_front->size);  				
				runner_front = GetNextMarker(runner_front);
			}
			
			/* enough memory is found after defragmentation */
			if (runner_back->size > (long)nbytes)
			{
				break;
			}	
		}
		
		/* move to next marker */
		runner_back = GetNextMarker(runner_back);
	}
	
	/* not enough memory is found for the allocation */
	if (INVALID == runner_back->size)
	{
		return NULL;
	}
	
	/* memory allocation success */
	return runner_back;	
}

