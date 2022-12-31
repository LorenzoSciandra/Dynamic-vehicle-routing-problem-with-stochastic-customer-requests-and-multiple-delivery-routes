

#ifndef _H_BAG
#define _H_BAG

#ifdef __cplusplus
	extern "C" {
#endif

/*
	Bag data structure
	It is an array of integer from 0 to n-1
	plus an array that contains the position (or index) of each
	integer in the first array

	If an item is not present, its index is -1

	made by Jean-Francois Cote 2010
*/


typedef struct
{
	int count;
	int maxCount;
	int * items;
	int * index;
} Bag;

typedef Bag * BagPtr;

//initialize the bag, AddZeroToN = 1 means we initialize the list with ints from 0 to N-1, 0 the bag is empty
void BagInitMem(BagPtr * b, int n, int AddZeroToN);
void BagInitZeroToN(BagPtr b);

BagPtr BagClone(BagPtr b);

void BagFree(BagPtr b);

void BagAddItem(BagPtr b, int i);
void BagRemoveItem(BagPtr b, int i);

void BagClear(BagPtr b, int AddZeroToN);

void BagSwap(BagPtr b, int i, int j);

int BagContains(BagPtr b, int i);

void BagPrint(BagPtr b);

unsigned long long BagGetProductPrimes(BagPtr b);

#ifdef __cplusplus
	}
#endif

#endif
