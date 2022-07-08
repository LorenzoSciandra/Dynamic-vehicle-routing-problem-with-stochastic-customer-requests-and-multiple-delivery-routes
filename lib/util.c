

#include "util.h"
#include <stdio.h> 
#include <stdlib.h> 


/* qsort int comparison function */
int util_int_cmp(const void *a, const void *b){return ( *(int*)a - *(int*)b );}
int util_int_cmp_desc(const void *a, const void *b){return ( *(int*)b - *(int*)a );}
int util_unsigned_int_cmp(const void *a, const void *b){return ( *(unsigned int*)a - *(unsigned int*)b );}
int util_double_cmp(const void *a, const void *b){return ( *(double*)a - *(double*)b );}
int util_ushort_int_cmp(const void *a, const void *b){return ( *(unsigned short int*)a - *(unsigned short int*)b ); }
int util_ull_cmp(const void *a, const void *b){return ( *(unsigned long long*)a - *(unsigned long long*)b );}
int util_ushort_int_decreasing_cmp(const void *a, const void *b){return ( *(unsigned short int*)b - *(unsigned short int*)a );}


void util_sort_ushort_int(unsigned short int * l, int n){qsort(l, n, sizeof(unsigned short int), util_ushort_int_cmp);}
void util_sort_ushort_int_decreasing(unsigned short int * l, int n){qsort(l, n, sizeof(unsigned short int), util_ushort_int_decreasing_cmp);}
void util_sort_int(int * l, int n){qsort(l, n, sizeof(int), util_int_cmp);}
void util_sort_int_desc(int * l, int n){qsort(l, n, sizeof(int), util_int_cmp_desc);}
void util_sort_unsigned_int(unsigned int * l, int n){qsort(l, n, sizeof(unsigned int), util_unsigned_int_cmp);}
void util_sort_double(double * l, double n){qsort(l, n, sizeof(double), util_double_cmp);}
void util_sort_ull(unsigned long long * l, int n){qsort(l, n, sizeof(unsigned long long), util_ull_cmp);}


int util_get_max_int(int * l, int n)
{
	int maxl = l[0];
	int i;
	for(i=1;i<n;i++)
		maxl = maxl<l[i]?l[i]:maxl;
	return maxl;
}

//calculate the maximal number of object that can fit in W
int util_calc_max_obj_in(int * w, int n, int W)
{
	util_sort_int(w,n);
	int i,sum=0;
	for(i=0;i<n && sum <= W;i++)
		sum += w[i];
	return i-1;
}

int util_calc_max_obj_in_with_maxl(int * w, int n, int W, int * maxl)
{
	util_sort_int(w,n);
	int i,sum=0;
	for(i=0;i<n && sum <= W;i++)
		sum += w[i];
	*maxl = w[i-1];
	return i-1;
}

//remove the elements that are equals and return the size of the new array
int util_remove_duplicates_int(int * l, int n)
{
	int last = 1;
	int i;
	for(i=1;i<n;i++)
		if(l[i-1] != l[i])
			l[last++] = l[i];
	return last;
}


int util_solve_knapsack(int * w, int n, int capacity)
{
	int i,j;
	//printf("util_solve_knapsack: capacity:%d\n", value);
	//printf("Items:");
	//for(i = 0 ; i <n;i++) printf("%d ", items[i]);
	//printf("\n");
	
	int ** B = (int**)malloc(sizeof(int*) * (n+1));
	for(i = 0 ; i <= n ; i++) B[i] = (int*)malloc(sizeof(int) * (capacity+1));
	for(i = 0 ; i <= capacity ; i++) B[0][i] = 0;
	for(i = 1 ; i <= n  ; i++)	B[i][0] = 0;
	
	for(i = 1; i<= n;i++)
		for(j=1;j<=capacity;j++)
		{
			if(j - w[i-1] >= 0 && B[i][j - w[i-1]] + w[i-1] > B[i-1][j])
				B[i][j] = B[i][j - w[i-1]] + w[i-1];
			else
				B[i][j] = B[i-1][j];
		}
		
	int result = B[n][capacity];
	
	//util_matrix_show_int(B,n+1, capacity+1);
	
	for(i = 0 ; i <= n ; i++) 
		free(B[i]);
	free(B);
	return result;
}

int util_solve_max_item_knapsack(int * w, int n, int W)
{
	int * pos = (int*)calloc(sizeof(int), W+1);
	pos[0] = 1;
	int i,j;

	/*printf("n:%d W:%d w:",n,W);
	for(i=0;i<n;i++)
		printf("%d ", w[i]);
	printf("\n");
	*/
	int last = 0;	
	for(i=0;i<n;i++)
	{	
		
		int a = W-w[i]>last?last:W-w[i];
		for(j=a;j>=0;j--)
			if(j + w[i] <= W && pos[j] > 0)
			{
				if(pos[ j + w[i] ] == 0 && j > 0)
					pos[ j + w[i] ] = pos[j]+1;
				else if (pos[ j + w[i] ] == 0)
					pos[ j + w[i] ] = pos[j];
				last = last < j + w[i]?j + w[i]:last;
			}
	}
	
	int max = 0;
	for(j=0;j<=W;j++)
		max = pos[j]>max?pos[j]:max;
	return max;
}

void util_matrix_show_int(int ** x, int n, int m)
{
	int i,j;
	for(i = 0 ; i < n ; i++)
	{
		for(j = 0 ; j < m ; j++)
			printf("%d ", x[i][j]);
		printf("\n");
	}	
}

void util_matrix_show_double(double ** x, int n, int m)
{
	int i,j;
	for(i = 0 ; i < n ; i++)
	{
		for(j = 0 ; j < m ; j++)
			printf("%.4lf ", x[i][j]);
		printf("\n");
	}	
}


void util_vector_double_show(double * x, int n)
{
	int j;
	for(j = 0 ; j < n ; j++)
		printf("%.4lf ", x[j]);
	printf("\n");
}


void util_vector_int_show(int * x, int n)
{
	int j;
	for(j = 0 ; j < n ; j++)
		printf("%d ", x[j]);
	printf("\n");	
}

void util_vector_unsigned_int_show(unsigned int * x, int n)
{
	int j;
	for(j = 0 ; j < n ; j++)
		printf("%u ", x[j]);
	printf("\n");	
}

void util_vector_unsigned_short_int_show(unsigned short int * x, int n)
{
	
	int j;
	for(j = 0 ; j < n ; j++)
		printf("%hu ", x[j]);
	printf("\n");	
}

void util_vector_set_int(int * x, int n, int value)
{
	int i;
	for(i=0;i<n;i++)
		x[i] = value;
}

void util_vector_set_double(double * x, int n, double value)
{
	int i;
	for(i=0;i<n;i++)
		x[i] = value;
}

void util_vector_copy_int(int * x, int * y, int n)
{
	int i;
	for(i=0;i<n;i++)
		x[i] = y[i];
}

void util_vector_or_int(int * x, int * y, int n) //or y into x
{
	int i;
	for(i=0;i<n;i++)
		x[i] |= y[i];
}

int util_linear_search(int n, int * x, int v)
{
	int i;
	for(i=0;i<n;i++)
		if(x[i] == v)
			return i;
	return -1;	
}

//search the value v in the vector x
//return the index of the v in the vector
//if v is not in x, it returns -1
//Remark : we take into account the the array x is sorted
int util_search(int n, int * x, int v)
{
	//if(n == 0) return -1;
	int first = 0;
	int last = n - 1;
	int middle = (first+last)/2;
	
	while( first <= last )
	{
		if (x[middle] < v)
			first = middle + 1;    
		else if (x[middle] == v) 
			break;
		else
			last = middle - 1;
		middle = (first + last)/2;
	}
	
	if(first > last)//item not found
		middle = -1;
	return middle;
}

int util_search_ull(int n, unsigned long long * x, unsigned long long v)
{
	if(n <= 0) return -1;
	
	int first = 0;
	int last = n - 1;
	int middle = (first+last)/2;

	
	while( first <= last )
	{
		if (x[middle] < v)
			first = middle + 1;    
		else if (x[middle] == v) 
			break;
		else
			last = middle - 1;
		middle = (first + last)/2;
	}
	
	if(first > last)//item not found
		middle = -1;
	return middle;
}


//check if p[v] is zero, if not, return the next position different than zero
//return the value
int util_not_zero_get_next(int * p,int v)
{
	if(p == NULL) return 0;
	if(p[v] == 0)
		return util_get_next(p,v);
	else
		return v;
}

//get the next position after v which is not zero
//return the value
int util_get_next(int * p, int v)
{
	while(p[++v] == 0);
	return v;
}



int util_count_bits(unsigned long long n) 
{     
	int c = 0; // c accumulates the total bits set in v
	for(; n; c++) 
		n &= n - 1; // clear the least significant bit set
	return c;
}

