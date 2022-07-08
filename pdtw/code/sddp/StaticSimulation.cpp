
#include "StaticSimulation.h"
#include "Decisions.h"
#include "DecisionMultiSet.h"
#include "Solver.h"
#include "SolverUlmer.h"
#include <time.h>
#include <limits.h>
#include "Parameters.h"
#include "../../../lib/Chrono.h"
#include <stdlib.h>

void StaticSimulation::Optimize(Scenarios & scenarios)
{
	srand( Parameters::GetSeed() );
	ChronoCpuNoStop chrono;chrono.start();
	nb_events = 1;

	Prob<Node, Driver> real;
	scenarios.GenerateReal(real, Parameters::GetTimeHorizon()+1);

	if(Parameters::GetInstanceType() != INSTANCE_TYPE_ULMER)
	{
		Solver solver(real);
		solver.alns_iteration_count = Parameters::GetStaticAlnsIterations();
		solver.Optimize();

		//solver.Show();

		cost = solver.cost;
		unassigneds = solver.nb_unassigneds/2;
		distance = solver.distances;
		time_taken = chrono.getTime();
		nb_routes = solver.GetRouteCount();
		future_assignments = solver.GetFutureCustomerAssignmentCount();
		future_customers = solver.GetFutureCustomerCount();
		known_unassignments = solver.GetInitialCustomerUnassignmentCount();
		solver.GetSolutionCompact(final_solution);
		//solver.Show();
		solver.GetReport(report);
	}
	else
	{
		SolverUlmer solver(real);
		solver.alns_iteration_count = Parameters::GetStaticAlnsIterations();
		solver.Optimize();

		//solver.Show();

		cost = solver.cost;
		unassigneds = solver.nb_unassigneds/2;
		distance = solver.distances;
		time_taken = chrono.getTime();
		nb_routes = solver.GetRouteCount();
		future_assignments = solver.GetFutureCustomerAssignmentCount();
		future_customers = solver.GetFutureCustomerCount();
		known_unassignments = solver.GetInitialCustomerUnassignmentCount();
		solver.GetSolutionCompact(final_solution);
	}
	final_solution.name = "Static";

	Show();
}
