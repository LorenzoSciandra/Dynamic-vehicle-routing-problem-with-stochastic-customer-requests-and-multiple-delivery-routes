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

#ifndef MATHFUNC_H
#define MATHFUNC_H

#ifdef __cplusplus
	extern "C" {
#endif

#define EPSILON 10E-5
#define INF   99999999
#define INFINITE 99999999.9

double MIN_DBL(double x, double y);
double MAX_DBL(double x, double y);
int MAX_INT(int x, int y);
int MIN_INT(int x, int y);
int CEIL_DIV(int x, int y);
int FLOOR_DIV(int x, int y);
int ABS_INT(int x);
double ABS_DBL(double x);

//return a random number between min(inclusivly) and max(exclusivly)
int mat_func_get_rand_int(int min, int max);

//return a random number between min(inclusivly) and max(exclusivly) and different than no
int mat_func_get_rand_int_diff(int min, int max, int no);

double 	mat_func_get_rand_double();
double 	mat_func_get_rand_double_between(double min, double max);
double 	mat_abs_diff(double x1, double x2, double y1, double y2);
double 	mat_func_get_max_smaller_than(double ** mat, int dim, double max_value);
float 	mat_func_get_max_smaller_than_f(float ** mat, int dim, float max_value);

double * math_func_get_double_array(int n);
double ** math_func_get_double_matrix(int n,int m);
void math_func_free_double_array(double * a);
void math_func_free_double_matrix(double ** a,int n);


double haversine_distance(double th1, double ph1, double th2, double ph2);


unsigned int jenkins_one_at_a_time_hash(char *key, int len);

//return the z value for which a 0-1 normal law P(Z < a) = p
double Inverse01NormalLaw(double p);

//return the a value for which a normal law X~N(u, s*s) is P(X < a) = p
double InverseNormalLaw(double u, double s, double p);


unsigned int nChoosek( unsigned int n, unsigned int k );

#ifdef __cplusplus
	}
#endif

#endif
