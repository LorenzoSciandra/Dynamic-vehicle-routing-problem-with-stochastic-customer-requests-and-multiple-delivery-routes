
#include "DynSimulation.h"
#include "Decisions.h"
#include "DecisionMultiSet.h"
#include "Solver.h"
#include <time.h>
#include <limits.h>
#include "Parameters.h"
#include "../../../lib/Chrono.h"
#include <stdlib.h>

void DynSimulation::Optimize(Scenarios & scenarios)
{
	srand( Parameters::GetSeed() );
	nb_events = 0;
	double cur_time = Parameters::GetTimeHorizonStartTime();
	_forbid_stochastic_drop_after_real = Parameters::ForbidStochasticDropAfterReal();
	_allow_en_route_returns = Parameters::AllowEnRouteDepotReturns();

	{
		double t = scenarios.GetNextEvent(cur_time);
		if(t > cur_time) //there are no customer at the beginning
			cur_time = t;
	}
	printf("Dynamic StartTime:%.1lf \n", cur_time);
	ChronoCpuNoStop chrono;chrono.start();

	Decisions prev_decisions;
	for(nb_events=0;cur_time <= Parameters::GetTimeHorizon();nb_events++)
	{
		Parameters::SetCurrentTime(cur_time);
		//printf("\n\n\nTime:%.1lf Unassigned:%d\n", cur_time, prev_decisions.GetUnassignedCount());
		//prev_decisions.Show();

		Prob<Node, Driver> prob;
		scenarios.GenerateReal(prob, cur_time);

		Solver solver(prob);
		solver.SetDecisions(prev_decisions);
		solver.Optimize();
		//solver.Show();
		//printf("Time:%d Dist:%.2lf Un:%d\n", (int)cur_time, solver.distances, solver.nb_unassigneds/2);

		Decisions decisions;
		solver.GetDecisions(decisions);


		//cur_time = scenarios.GetNextEvent(cur_time);
		cur_time = GetNextEvent(scenarios, decisions, cur_time);
		//printf("Next Time:%.1lf \n", cur_time);
		prev_decisions = decisions;
		//decisions.Show();
		prev_decisions.Remove(cur_time);
		//printf("\n\n");
		//prev_decisions.Show();
		//getchar();
	}



	Prob<Node, Driver> real;
	scenarios.GenerateReal(real, Parameters::GetTimeHorizon()+1);
	Parameters::SetCurrentTime(0);
	Solver solver(real);
	solver.SetDecisions(prev_decisions);
	//solver.Optimize();
	solver.Unfix();
	//solver.Show();
	//prev_decisions.Show();

	solver.Update();
	cost = solver.cost;
	unassigneds = solver.nb_unassigneds/2;
	distance = solver.distances;
	nb_routes = solver.GetRouteCount();
	time_taken = chrono.getTime();

	//Show();
	//printf("FutureCustomerAssignments:%d\n", );
	//printf("KnownCustomerUnassignments:%d\n", solver.GetInitialCustomerUnassignmentCount());

	printf("Dynamic ");
	printf("Cost:%.2lf Dist:%.2lf Un:%d Routes:%d ", cost, distance, unassigneds, nb_routes);
	printf("Time:%.3lf Events:%d\n", time_taken,nb_events);

	final_solution.name = "Dynamic";
	if(!_forbid_stochastic_drop_after_real)
		final_solution.name.append(" WS");
	if(_allow_en_route_returns)
		final_solution.name.append(" PDR");
	solver.GetSolutionCompact(final_solution);
	//solver.Show();
	solver.GetReport(report);
}

double DynSimulation::GetNextEvent(Scenarios & scenarios, Decisions & decs, double cur_time)
{
	double next_request = scenarios.GetNextEvent(cur_time);
	if(!Parameters::AllowEnRouteDepotReturns()) return next_request;

	double next_delivery = Parameters::GetTimeHorizon()+999;
	for(int i=0;i<decs.GetCount();i++)
	{
		Decision * d = decs.Get(i);
		if(d->decision_type != DECISION_TYPE_ROUTING) continue;
		if(!d->is_real) continue;

		if(d->node_type == NODE_TYPE_DROP && d->departure_time > cur_time)
			next_delivery = std::min(next_delivery, d->departure_time);
	}

	return std::min(next_request, next_delivery);
}
