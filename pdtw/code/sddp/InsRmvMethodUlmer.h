/*
 * Copyright Jean-Francois Cote 2019
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
*/

#ifndef INSRMV_ULMER
#define INSRMV_ULMER

#include "NodeSddp.h"
#include "DriverSddp.h"
#include "../Move.h"
#include "../InsRmvMethod.h"
#include "../ProblemDefinition.h"
#include "CostFunctionSddp.h"
#include "InsRmvMethodSddp.h"


class InsRmvMethodUlmer : public InsRmvMethod<Node, Driver, MoveSddp>
{
	public:
		InsRmvMethodUlmer(Prob<Node,Driver> * prob, CostFunctionSddp & cost_func);
		~InsRmvMethodUlmer();

		void InsertCost(Sol<Node,Driver> & s, Node * n, Driver * d, Move<Node,Driver,MoveSddp> & m);
		void InsertCostUlmer(Sol<Node,Driver> & s, Node * n, Driver * d, Move<Node,Driver,MoveSddp> & m);

		void ApplyInsertMove(Sol<Node,Driver> & s, Move<Node,Driver,MoveSddp> & m);

		void RemoveCost(Sol<Node,Driver> & s, Node * n, Move<Node,Driver,MoveSddp> & m);

		void CheckMove(Sol<Node,Driver> & s, Move<Node,Driver,MoveSddp> & m){};

		bool show_output;
	private:
		Prob<Node,Driver> * p;
		double _added_noise;
		CostFunctionSddp _cost_func;
};
#endif
