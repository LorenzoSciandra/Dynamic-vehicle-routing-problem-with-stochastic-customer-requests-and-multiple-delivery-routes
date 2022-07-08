

#include "bag.h"
#include "primes.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void BagInitMem(BagPtr * bag, int n, int AddZeroToN)
{
	BagPtr b = (BagPtr)malloc(sizeof(Bag));
	(*bag) = b;
	b->maxCount = n;
	b->items = (int*)malloc(sizeof(int)* n);
	b->index = (int*)malloc(sizeof(int)* n);

	int i;
	if(AddZeroToN != 0)
	{
		b->count = b->maxCount;
		for(i=0;i<b->maxCount;i++)
		{
			b->items[i]=i;
			b->index[i]=i;
		}
	}
	else
	{
		b->count = 0;
		for(i=0;i<b->maxCount;i++)
			b->index[i]=-1;
	}
}

void BagInitZeroToN(BagPtr b)
{
	int i;
	b->count = b->maxCount;
	for(i=0;i<b->maxCount;i++)
	{
		b->items[i]=i;
		b->index[i]=i;
	}
}

BagPtr BagClone(BagPtr b)
{
	BagPtr new_bag;
	BagInitMem(&new_bag,b->maxCount,0);
	new_bag->count = b->count;
	new_bag->maxCount = b->maxCount;

	int i;
	for(i=0;i<b->maxCount;i++)
	{
		new_bag->items[i] = b->items[i];
		new_bag->index[i] = b->index[i];
	}
	return new_bag;
}


void BagFree(BagPtr b)
{
	if(b != NULL)
	{
		free(b->items);
		free(b->index);
		free(b);
	}
}

int BagContains(BagPtr b, int i)
{
	return b->index[i] != -1;
}
void BagClear(BagPtr b, int AddZeroToN)
{
	int i;
	if(AddZeroToN)
	{
		b->count = b->maxCount;
		for(i=0;i<b->maxCount;i++)
		{
			b->items[i]=i;
			b->index[i]=i;
		}
	}
	else
	{
		for(i=0;i<b->count;i++)
			b->index[ b->items[i] ] = -1;
		b->count = 0;
		//for(i=0;i<b->maxCount;i++)
		//	b->index[i]=-1;
	}
}

void BagAddItem(BagPtr b, int i)
{
	if(b->index[i] == -1 && i < b->maxCount && b->count < b->maxCount)
	{
		b->index[i] = b->count;
		b->items[b->count] = i;
		b->count++;
	}
	else
	{
		printf("Maximum capacity reached or item already in %d count:%d maxcount:%d\n", i,b->count,b->maxCount);
		printf("Adding:%d\n", i);
		BagPrint(b);

		BagPtr b1 = NULL;
		b1->count = 0;
		exit(1);
	}
}

void BagRemoveItem(BagPtr b, int i)
{
	//printf("Removing:%d\titems left:", i);
	//int j;
	//for(j=0;j<b->count;j++) printf("%d(%d) ", b->items[j], b->index[ b->items[j]]);
	//printf("\n");


	if(b->index[i] != -1 && i < b->maxCount && b->count > 0)
	{
		b->items[ b->index[i] ] = b->items[ b->count-1 ];
		b->index[ b->items[ b->count-1 ] ] = b->index[i];
		b->index[i] = -1;
		b->count--;
	}
	else
	{
		printf("Removing non existing item from the bag %d count:%d\n", i,b->count);
		BagPtr b1 = NULL;
		b1->count = 0;
		exit(1);
	}

	}

void BagSwap(BagPtr b, int i, int j)
{
	b->items[ b->index[i] ] = j;
	b->items[ b->index[j] ] = i;
	int temp = b->index[i];
	b->index[i] = b->index[j];
	b->index[j] = temp;
}

void BagPrint(BagPtr b)
{
	printf("Bag content:");
	int i;
	for(i=0;i<b->count;i++)
		printf("%d ", b->items[i]);
	printf("\n");
}

unsigned long long BagGetProductPrimes(BagPtr b)
{
	unsigned long long l = 1;
	int i;
	for(i=0;i<b->count;i++)
		l *= prime_get_ith(b->items[i]);
	return l;
}
