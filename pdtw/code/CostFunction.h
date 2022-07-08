/*
 * Copyright Jean-Francois Cote 2012
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
 *
*/



#ifndef COST_FUNCTION
#define COST_FUNCTION

#include "ProblemDefinition.h"

template <class NodeT, class DriverT> class Sol;


template <class NodeT, class DriverT>

class CostFunction
{
	public:
		CostFunction(){}
		~CostFunction(){}

		//return only the cost of a route or a solution
		//no variables are updated
		virtual double GetCost(Sol<NodeT,DriverT> & s) = 0;
  		virtual double GetCost(Sol<NodeT,DriverT> & s, DriverT * d) = 0;

		//return the cost of a route or a solution
		//and update temporary variables
		virtual double Update(Sol<NodeT,DriverT> & s) = 0;
		virtual double Update(Sol<NodeT,DriverT> & s, DriverT * d) = 0;

		virtual void Show(Sol<NodeT,DriverT> * s, DriverT * d){}
		virtual void Show(Sol<NodeT,DriverT> * s)
		{
			printf("Please implement the show method\n");
		}

		virtual void MakeFeasible(Sol<NodeT,DriverT> & s, DriverT * d){}
};


#endif
