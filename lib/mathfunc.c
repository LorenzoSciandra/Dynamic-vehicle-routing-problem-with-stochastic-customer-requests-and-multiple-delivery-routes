
#include "mathfunc.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

double MAX_DBL(double x, double y)
{
	if(x < y)
		return y;
	else
		return x;
}
int MAX_INT(int x, int y) {return (x>y)?x:y;}
int MIN_INT(int x, int y) {return (x<y)?x:y;}
int CEIL_DIV(int x, int y){return (int)ceil( ((double)x)/((double)y));}
int FLOOR_DIV(int x, int y){return (int) ((double)x / (double)y );}
//inline int MAX_INT(int x, int y)
//{
	//if(x < y)
	//	return y;
	//else
	//	return x;
//	return x ^ ((x ^ y) & -(x < y));
//}

//inline int MIN_INT(int x, int y)
//{
	//if(x > y)
	//	return y;
	//else
	//	return x;
//	return y ^ ((x ^ y) & -(x < y));
//}

double MIN_DBL(double x, double y)
{
	if(x > y)
		return y;
	else
		return x;
}



int ABS_INT(int x)
{
	//int mask = x >> 31;
	//return ((-x) & mask) | (x & ~mask);

	if(x > 0)
		return x;
	else
		return -x;

}
double ABS_DBL(double x)
{
	if(x > 0)
		return x;
	else
		return -x;
}

//return a random number between min(inclusivly) and max(exclusivly)
int mat_func_get_rand_int(int min, int max)
{
	if(min == max) return min;
	return (rand() % (max - min)) + min;
}

int mat_func_get_rand_int_diff(int min, int max, int no)
{
	int cpt = 0;
	int n = mat_func_get_rand_int(min, max);
	while(n == no && cpt++ < 100)
		n = mat_func_get_rand_int(min, max);

	return n;
}

double mat_func_get_rand_double()
{
	return rand() / (double)RAND_MAX;
}

double mat_abs_diff(double x1, double x2, double y1, double y2)
{
	return ABS_DBL(x1-x2) + ABS_DBL(y1-y2);
}

double mat_func_get_max_smaller_than(double ** mat, int dim, double max_value)
{
	double maxv = -INFINITE;
	int i,j;
	for(i = 0 ; i < dim ; i++)
		for(j = 0 ; j < dim ; j++)
			if(mat[i][j] > maxv && mat[i][j] < max_value)
				maxv = mat[i][j];
	return maxv;
}
float mat_func_get_max_smaller_than_f(float ** mat, int dim, float max_value)
{
	float maxv = -INFINITE;
	int i,j;
	for(i = 0 ; i < dim ; i++)
		for(j = 0 ; j < dim ; j++)
			if(mat[i][j] > maxv && mat[i][j] < max_value)
				maxv = mat[i][j];
	return maxv;
}

double 	mat_func_get_rand_double_between(double min, double max)
{
	return (max-min) * mat_func_get_rand_double() + min;
}

double * math_func_get_double_array(int n)
{
	return (double*)malloc(sizeof(double)*n);
}
double ** math_func_get_double_matrix(int n,int m)
{
	double ** p = (double**)malloc(sizeof(double*)*n);
	int i;
	for(i=0;i<n;i++)
		p[i] = (double*)malloc(sizeof(double)*m);
	return p;
}
void math_func_free_double_array(double * a)
{
	free(a);
}
void math_func_free_double_matrix(double ** a,int n)
{
	int i=0;
	for(i=0;i<n;i++)
		if(a[i] != NULL)
			free(a[i]);
	free(a);
}

//can be used this way
//haversine_distance(n1->x,n1->y,n2->x,n2->y);
//th1 is latitude and ph1 is longitude
double haversine_distance(double th1, double ph1, double th2, double ph2)
{
	double dx, dy, dz;
	ph1 -= ph2;
	ph1 *= (3.1415926536 / 180);
	th1 *= (3.1415926536 / 180);
	th2 *= (3.1415926536 / 180);

	dx = cos(ph1) * cos(th1) - cos(th2);
	dy = sin(ph1) * cos(th1);
	dz = sin(th1) - sin(th2);
	return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * 6371;
}

unsigned int jenkins_one_at_a_time_hash(char *key, int len)
{
    unsigned int hash, i;
    for(hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

//code taken from https://www.johndcook.com/blog/cpp_phi_inverse/
double RationalApproximation(double t)
{
    // Abramowitz and Stegun formula 26.2.23.
    // The absolute value of the error should be less than 4.5 e-4.
    double c[] = {2.515517, 0.802853, 0.010328};
    double d[] = {1.432788, 0.189269, 0.001308};
    return t - ((c[2]*t + c[1])*t + c[0]) /
               (((d[2]*t + d[1])*t + d[0])*t + 1.0);
}

double Inverse01NormalLaw(double p)
{
    if (p <= 0.0 || p >= 1.0)
    {
		 printf("NormalCDFInverse Invalid input argument:%lf\n", p);
		 printf("must be larger than 0 but less than 1.\n");
		 exit(1);
    }
    // See article above for explanation of this section.
    if (p < 0.5)
    {
        // F^-1(p) = - G^-1(p)
        return -RationalApproximation( sqrt(-2.0*log(p)) );
    }
    else
    {
        // F^-1(p) = G^-1(1-p)
        return RationalApproximation( sqrt(-2.0*log(1-p)) );
    }
}
double InverseNormalLaw(double u, double s, double p)
{
	return Inverse01NormalLaw(p) * s + u;
}

unsigned int nChoosek( unsigned int n, unsigned int k )
{
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;

    int result = n;
    for( int i = 2; i <= k; ++i ) {
        result *= (n-i+1);
        result /= i;
    }
    return result;
}
