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

#ifndef IALGORITHM
#define IALGORITHM

#include "constants.h"
#include "Solution.h"
#include "ISolutionList.h"
#include <vector>
#include <map>

template<class NodeT, class DriverT>
class IAlgorithm {
public :
    IAlgorithm() {}

    virtual ~IAlgorithm() {}

    virtual void Optimize(Sol<NodeT, DriverT> &sol) = 0;

    virtual void Optimize(Sol<NodeT, DriverT> &sol, ISolutionList<NodeT, DriverT> *list) {};

    virtual void SetMaxTime(int maxtime) {}
};

#endif




