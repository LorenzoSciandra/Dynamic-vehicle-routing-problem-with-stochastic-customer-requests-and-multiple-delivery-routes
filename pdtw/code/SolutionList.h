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

#ifndef SOLUTIONLIST
#define SOLUTIONLIST

#include "Solution.h"

template <class NodeT, class DriverT>
class SolutionList
{
	public : 
		SolutionList(){}
		virtual ~SolutionList(){}
		virtual void Add(Sol<NodeT,DriverT> & s) = 0;
		virtual Prob<NodeT,DriverT>* GetProblemDefinition() = 0;
		virtual int GetSolutionCount() = 0;
		virtual Sol<NodeT,DriverT>* GetSolution(int i) = 0;
};

#endif
