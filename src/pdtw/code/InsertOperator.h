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

#ifndef INSERT_OPERATOR
#define INSERT_OPERATOR

#include "Solution.h"

template<class NodeT, class DriverT>
class InsertOperator {
public:
    InsertOperator() {}

    virtual ~InsertOperator() {}

    virtual void Insert(Sol<NodeT, DriverT> &s) = 0;
};

#endif