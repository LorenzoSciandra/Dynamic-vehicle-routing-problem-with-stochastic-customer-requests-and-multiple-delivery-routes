#ifndef SOLVER_SDDP
#define SOLVER_SDDP

#include <stdio.h>
#include <stdlib.h>
#include "../Solution.h"
#include "../ProblemDefinition.h"
#include "NodeSddp.h"
#include "DriverSddp.h"
#include "CostFunctionSddp.h"
#include "InsRmvMethodSddp.h"
#include "Decisions.h"
#include "../SolCompact.h"
#include "Report.h"

using namespace std;

class Solver
{

private:
	Sol<Node, Driver> sol;
	CostFunctionSddp cost_func;
	Prob<Node, Driver> & prob;
	InsRmvMethodSddp method;

public:
	Solver(Prob<Node, Driver> & pr);
	void Optimize();
	void GetDecisions(Decisions & d);
	void SetDecisions(Decisions & d);
	void Show(){sol.Show();}
	void Update();
	void GetSolutionCompact(SolCompact<Node,Driver> & s);
	void GetReport(Report & report);

	//Just recalculate the cost of all routes where a wait action is performed for each request to be delivered in the future
	//return the amount of wait that can be done
	int EvaluateAllWait();

	void Check();

	void Unfix();//unfix all decisions (mostly for recalculating the cost of the solution)

	//When there is a set of consecutive pickups, we want to make
	//sure real pickups aren't picked up by the driver in the case
	//there are some stochastic pickups right after them
	//the lastest stochastic pickup is moved before the first pickups
	void MoveLatestPickupFirst();


	//calculate the number of times we return to the depot
	int GetRouteCount();

	//return the number of customers with a release_time > 0 that are assigned
	int GetFutureCustomerAssignmentCount();
	int GetFutureCustomerCount();
	int GetInitialCustomerUnassignmentCount();

	int alns_iteration_count;
	double cost;
	int nb_unassigneds;
	int nb_real_unassigneds;
	double distances;
};



#endif
