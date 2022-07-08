/*
 * Copyright Jean-Francois Cote 2012
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
*/

#ifndef BITSET_H
#define BITSET_H


#ifdef __cplusplus
	extern "C" {
#endif


typedef struct
{
	unsigned int size;
	unsigned int * bits;
} bitset_t;


typedef bitset_t * bitset_ptr;

bitset_ptr bitset_init(unsigned int nbbits);
void bitset_free(bitset_ptr b);

void bitset_setbit(bitset_ptr b, unsigned int bit);
void bitset_unsetbit(bitset_ptr b, unsigned int bit);
void bitset_clear(bitset_ptr b);
void bitset_setall(bitset_ptr b);

//return 1 if the bit is set, 0 otherwise
int bitset_isset(bitset_ptr b, unsigned int bit);



#ifdef __cplusplus
	}
#endif

#endif
