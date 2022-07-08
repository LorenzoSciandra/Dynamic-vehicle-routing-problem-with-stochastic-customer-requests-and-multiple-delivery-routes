/*
 * Copyright Jean-Francois Cote 2012
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
 *
 *
 * This is a dirty set of functions..
*/

#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
	extern "C" {
#endif

int util_get_max_int(int * l, int n);
void util_sort_int(int * l, int n);
void util_sort_int_desc(int * l, int n);
void util_sort_unsigned_int(unsigned int * l, int n);
void util_sort_ull(unsigned long long * l, int n);
void util_sort_unsigned_short_int(unsigned short int * l, int n);
void util_sort_unsigned_short_int_decreasing(unsigned short int * l, int n);
void util_sort_double(double * l, double n);

int util_solve_knapsack(int * items, int n, int capacity);
int util_solve_max_item_knapsack(int * w, int n, int W);

void util_matrix_show_int(int ** x, int n, int m);
void util_matrix_show_double(double ** x, int n, int m);


void util_vector_double_show(double * x, int n);
void util_vector_int_show(int * x, int n);
void util_vector_unsigned_int_show(unsigned int * x, int n);
void util_vector_unsigned_short_int_show(unsigned short int * x, int n);

void util_vector_set_int(int * x, int n, int value);
void util_vector_set_double(double * x, int n, double value);
void util_vector_copy_int(int * x, int * y, int n); //copy y into x
void util_vector_or_int(int * x, int * y, int n); //or y into x

//remove the elements that are equals and return the size of the new array
int util_remove_duplicates_int(int * l, int n);

//calculate the maximal number of object that can fit in W
int util_calc_max_obj_in(int * l, int n, int W);

int util_calc_max_obj_in_with_maxl(int * l, int n, int W, int * maxl);

//heuristic of Letchford to find a cover inequality
int util_heur_calc_cover(int * w, double * x, int n, int W);



//search the value v in the vector x
//return the index of the v in the vector
//if v is not in x, it returns -1
//Remark : we take into account that the array x is sorted
int util_search(int n, int * x, int v);
int util_search_ull(int n, unsigned long long * x, unsigned long long v);

//search the value v in the vector x
//return the index of the v in the vector
//if v is not in x, it returns -1
//Remark : we do not take into account that the array x is sorted
int util_linear_search(int n, int * x, int v);

//check if p[v] is zero, if not, return the next position different than zero
//return the value
//remark : v should always be smaller than the size of p
int util_not_zero_get_next(int * p, int v);

//get the next position after v which is not zero
//return the value
//remark : v should always be smaller than the size of p
int util_get_next(int * p, int v);

//return the number of bits set to 1
int util_count_bits(unsigned long long n);

#ifdef __cplusplus
	}
#endif

#endif

