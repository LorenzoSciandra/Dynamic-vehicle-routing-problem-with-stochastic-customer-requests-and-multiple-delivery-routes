
#include "DynSimulationUlmer.h"
#include "Decisions.h"
#include "DecisionMultiSet.h"
#include "SolverUlmer.h"
#include <time.h>
#include <limits.h>
#include "Parameters.h"
#include "../../../lib/Chrono.h"
#include <stdlib.h>

void DynSimulationUlmer::Optimize(Scenarios & scenarios)
{
	if(Parameters::GetInstanceType() != INSTANCE_TYPE_ULMER)
	{
		printf("The instance data should be from Ulmer's\n");
		exit(1);
	}

	srand( Parameters::GetSeed() );
	nb_events = 0;
	double cur_time = Parameters::GetTimeHorizonStartTime();
	{
		double t = scenarios.GetNextEvent(cur_time);
		if(t > cur_time) //there are no customer at the beginning
			cur_time = t;
	}
	printf("Dynamic Ulmer Depot:%d StartTime:%.1lf \n", Parameters::GetUlmerDepotLocation(), cur_time);
	ChronoCpuNoStop chrono;chrono.start();

	Decisions prev_decisions;
	for(int i=0;i<scenarios.GetRealScenarioCustomerCount();i++)
	{
		Node * n = scenarios.GetRealScenarioCustomer(i);
		if(n->type == NODE_TYPE_PICKUP && n->release_time <= 0.01)
		{
			Decision d;
			d.req_id = prev_decisions.GetCount();
			d.decision_type = DECISION_TYPE_ACTION;
			d.action_type = DECISION_ACTION_DONT_UNASSIGN;
			prev_decisions.Add(d);
		}
	}



	for(nb_events=0;cur_time <= Parameters::GetTimeHorizon();nb_events++)
	{
		Parameters::SetCurrentTime(cur_time);
		//printf("\n\n\nTime:%.1lf Unassigned:%d\n", cur_time, prev_decisions.GetUnassignedCount());
		//prev_decisions.Show();

		Prob<Node, Driver> prob;
		scenarios.GenerateReal(prob, cur_time);

		SolverUlmer solver(prob);
		solver.SetDecisions(prev_decisions);
		solver.Optimize();
		//solver.Show();
		//printf("Time:%d Dist:%.2lf Un:%d\n", (int)cur_time, solver.distances, solver.nb_unassigneds/2);
		if(cur_time <= 0.01 && solver.nb_unassigneds >= 1)
		{
			break;
		}


		Decisions decisions;
		solver.GetDecisions(decisions);


		//cur_time = scenarios.GetNextEvent(cur_time);
		cur_time = GetNextEvent(scenarios, decisions, cur_time);

		//printf("Next Time:%.1lf \n", cur_time);
		//decisions.Show();
		prev_decisions = decisions;
		//decisions.Show();
		prev_decisions.Remove(cur_time);
		//prev_decisions.Show();
	}



	Prob<Node, Driver> real;
	scenarios.GenerateReal(real, Parameters::GetTimeHorizon()+1);
	Parameters::SetCurrentTime(0);
	SolverUlmer solver(real);
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
	future_assignments = solver.GetFutureCustomerAssignmentCount();
	future_customers = solver.GetFutureCustomerCount();
	known_unassignments = solver.GetInitialCustomerUnassignmentCount();
	//Show();
	//printf("FutureCustomerAssignments:%d\n", );
	//printf("KnownCustomerUnassignments:%d\n", solver.GetInitialCustomerUnassignmentCount());

	printf("Dynamic ");
	printf("Cost:%.2lf Dist:%.2lf FC:%d KU:%d FA:%d ", cost, distance, future_customers, known_unassignments,future_assignments);
	printf(" Routes:%d Time:%.3lf Events:%d\n", nb_routes,time_taken,nb_events);
}



double DynSimulationUlmer::GetNextEvent(Scenarios & scenarios, Decisions & decs, double cur_time)
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
