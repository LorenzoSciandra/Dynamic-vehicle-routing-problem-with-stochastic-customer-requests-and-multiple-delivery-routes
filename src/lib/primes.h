/*
 * Copyright Jean-Francois Cote 2012
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
*/

#ifndef PRIMES
#define PRIMES

#ifdef __cplusplus
	extern "C" {
#endif


#define PRIME_NUMBERS_COUNT 1000

int prime_get_ith(int i);

void prime_build_set(int biggest_num);

void prime_show();

#ifdef __cplusplus
	}
#endif

#endif
