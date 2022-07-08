

#include "bitset.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h> 

#define BITS_PER_INT sizeof(unsigned int)
#define FULL 0xFFFF

bitset_ptr bitset_init(unsigned int nbbits)
{
	bitset_ptr b = (bitset_ptr)malloc(sizeof(bitset_t));
	b->size = nbbits/BITS_PER_INT;
	b->size += ((nbbits%BITS_PER_INT) != 0);
	//printf("nbbits:%u\n", b->size);
	b->bits = (unsigned int*)calloc(b->size, sizeof(unsigned int));
	return b;
}
void bitset_free(bitset_ptr b)
{
	if(b != NULL)
	{
		if(b->bits != NULL) 
			free(b->bits);
		free(b);	
	}
}

void bitset_setbit(bitset_ptr b, unsigned int bit)
{
	unsigned int index = bit / BITS_PER_INT;
	unsigned int pos = 1 << bit % BITS_PER_INT;
	
	//printf("bitset_setbit: index:%u pos:%u size:%u\n", index,pos, b->size);
	if(index >= b->size)
	{
		printf("bitset_setbit: Invalid bit position : index:%u size:%u\n", index, b->size);
		exit(1);
	}
	
	b->bits[index] |= pos;
}

void bitset_unsetbit(bitset_ptr b, unsigned int bit)
{
	unsigned int index = bit / BITS_PER_INT;
	unsigned int pos = 1 << bit % BITS_PER_INT;
	
	if(index >= b->size)
	{
		printf("bitset_setbit: Invalid bit position : index:%d size:%d\n", index, b->size);
		exit(1);
	}
	
	b->bits[index] &= ~pos;
}

void bitset_clear(bitset_ptr b)
{
	unsigned int i;
	for(i=0;i<b->size;i++)
		b->bits[i] = 0;
}

void bitset_setall(bitset_ptr b)
{
	unsigned int i;
	for(i=0;i<b->size;i++)
		b->bits[i] = FULL;
}


//return 1 if the bit is set, 0 otherwise
int bitset_isset(bitset_ptr b, unsigned int bit)
{
	unsigned int index = bit / BITS_PER_INT;
	unsigned int pos = 1 << bit % BITS_PER_INT;
	//unsigned int v1 = b->bits[index] & pos;
	//int v2 = b->bits[index] & pos;
	//int v3 =  v2 != 0?1:0;
	//printf("bitset_isset: index:%u pos:%u size:%u v:%u %d %d\n", index,pos, b->size,v1,v2, v3);
	return (b->bits[index] & pos) != 0?1:0;
}






