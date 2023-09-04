/************************************************ 
EXERCISE      : Variable Size Allocator
Implmented by : Snir Holland
Reviewed by   : Asaf Talker    
Date          : 24/5/2023
File          : vsa.h
*************************************************/

#ifndef __VSA_H__ 
#define __VSA_H__

#include <stddef.h>

typedef struct header vsa_t;

/*
*	Initializes the VSA
*
*	Arguments:
*		memory - memory to manage. must be a valid adress and aligned to WORD SIZE.
*		pool_size - size in bytes of the memory pool
*
*	Return:	pointer to the VSA
*
*	Time complexity: O(1) best/average/worst
*	Space complexity: O(1) best/average/worst
*/
vsa_t *VSAInit(void *memory, size_t pool_size); 

/*
*	Allocates a block of memory
*
*	Arguments:
*		vsa - pointer to the allocator. must be valid.
*		nbytes - number of bytes to allocate.
*	Return:	pointer to the allocated memory
*
*	Time complexity: O(1) best, O(n) average/worst
*	Space complexity: O(1) best/average/worst
*/
void *VSAAlloc(vsa_t *vsa, size_t nbytes);

/*
*	frees a block of memory
*
*	Arguments:
*		block - the block of memory to free. must belong to a VSA.
*
*	Return:	void
*
*	Time complexity: O(1) best/average/worst
*	Space complexity: O(1) best/average/worst
*/
void VSAFree(void *block);

/*
*	returns the size of the largest block available in the VSA 
*
*	Arguments:
*		vsa - pointer to the allocator. must be valid.
*		
*	Return - the size of the largest block in bytes
*
*	Time complexity: O(1) best, O(n) average/worst
*	Space complexity: O(1) best/average/worst
*/
size_t VSALargestChunkAvailabe(vsa_t *vsa);

#endif /* __VSA_H__ */
