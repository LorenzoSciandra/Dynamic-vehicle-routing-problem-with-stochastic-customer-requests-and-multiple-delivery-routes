/*
 * Copyright Jean-Francois Cote 2019
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
*/

#ifndef COST_FUNCTION_SDDP
#define COST_FUNCTION_SDDP

#include "NodeSddp.h"
#include "DriverSddp.h"
#include "../Solution.h"
#include "../ProblemDefinition.h"
#include "../CostFunction.h"

class CostFunctionSddp : public CostFunction<Node, Driver> {
public:
    CostFunctionSddp(Prob<Node, Driver> *prob) : p(prob) {}

    ~CostFunctionSddp() {}

    double GetCost(Sol<Node, Driver> &s);

    double GetCost(Sol<Node, Driver> &s, Driver *d);

    double Update(Sol<Node, Driver> &s);

    double Update(Sol<Node, Driver> &s, Driver *d);

    //Similar to Update but push the arrival time the lastest possible to reducing waiting
    double FinalUpdate(Sol<Node, Driver> &s);

    double FinalUpdate(Sol<Node, Driver> &s, Driver *d);

    void Show(Sol<Node, Driver> *s, Driver *d);

    void Show(Sol<Node, Driver> *s);

    void MakeFeasible(Sol<Node, Driver> &s, Driver *d);

private:
    Prob<Node, Driver> *p;

};


#endif
