/************************************************ 
EXERCISE      : Variable Size Allocator
Implmented by : Snir Holland
Reviewed by   : Asaf Talker    
Date          : 24/5/2023
File          : vsa_test.c
*************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "vsa.h"

/* Test Functions */
static void TestVSAInit();
static void TestVSAAlloc();
static void TestVSAFree();
static void TestVSALargestChunkAvailabe();

int main()
{

	TestVSAInit();

	TestVSAAlloc();

	TestVSAFree();

	TestVSALargestChunkAvailabe();
		
	return(0);
}

/*-------------TestVSAInit-------------*/
static void TestVSAInit()
{
	size_t pool_size = 160;

	void *buf = malloc(pool_size);
	
	vsa_t *vsa = VSAInit(buf, pool_size);

	printf("--------Testing VSAInit()------------------------\t");
	
	(NULL != vsa) ? printf("Success\n") : printf("Failure\n");
		
	free(buf);	

}

/*-------------TestVSAAlloc-------------*/
static void TestVSAAlloc()
{
	size_t pool_size = 160;
	
	void *buf = malloc(pool_size);
	
	vsa_t *vsa = VSAInit(buf, pool_size);
	
	void *memory = NULL;
	
	size_t largest_chunk_available = 0;

	printf("--------Testing VSAAlloc()------------------------\t");
	
	memory = VSAAlloc(vsa , 32);
	
	largest_chunk_available = VSALargestChunkAvailabe(vsa);
	
	(80 == largest_chunk_available) ? printf("Success\n") : printf("Failure\n");
	
	(void)memory;
	
	free(buf);
}

/*-------------TestVSAFree-------------*/
static void TestVSAFree()
{
	size_t pool_size = 160;
	
	void *buf = malloc(pool_size);
	
	vsa_t *vsa = VSAInit(buf, pool_size);
	
	void *memory = NULL;
	
	size_t largest_chunk_available = 0;

	printf("--------Testing VSAFree()-----------------------\t");
	
	memory = VSAAlloc(vsa , 32);
	
	VSAFree(memory);
	
	largest_chunk_available = VSALargestChunkAvailabe(vsa);

	(128 == largest_chunk_available) ? printf("Success\n") : printf("Failure\n");
	
	free(buf);
}

/*-------------TestVSALargestChunkAvailable-------------*/
static void TestVSALargestChunkAvailabe()
{
	size_t pool_size = 160;
	
	void *buf = malloc(pool_size);
	
	vsa_t *vsa = VSAInit(buf, pool_size);
	
	void *memory = NULL;
	
	size_t largest_chunk_available = 0;

	printf("--------Testing VSALargestChunkAvailabe()-----------\t");
	
	memory = VSAAlloc(vsa , 64);
	
	largest_chunk_available = VSALargestChunkAvailabe(vsa);
	VSAFree(memory);
	
	(48 == largest_chunk_available) ? printf("Success\n") : printf("Failure\n");
	
	free(buf);
}





