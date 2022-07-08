#ifndef STATIC_SIMULATION_H
#define STATIC_SIMULATION_H

#include "Scenarios.h"
#include "Decisions.h"
#include "Parameters.h"
#include "../SolCompact.h"
#include "Report.h"
#include "Test.h"

class StaticSimulation
{
public:

	double cost;
	double time_taken;
	int unassigneds;
	double distance;
	int nb_routes;
	int nb_events;
	int future_assignments;
	int future_customers;
	int known_unassignments;
	SolCompact<Node,Driver> final_solution;
	Report report;
	StaticSimulation(){}


	void Optimize(Scenarios & scenarios);

	void Show()
	{
		printf("StaticSimulation ");
		printf("Cost:%.2lf Dist:%.2lf Un:%d Routes:%d ", cost, distance, unassigneds, nb_routes);
		printf("Time:%.2lf\n", time_taken);
	}

	void ShowReport()
	{
		report.Show();
	}

	result GetResult()
	{
		result r;
		r.id = 1;
		r.nb_customers = Parameters::GetRealRequestCount();
		r.distance = distance;
		r.unassigneds = unassigneds;
		r.time_taken = time_taken;
		r.nb_routes = nb_routes;
		r.nb_events = nb_events;
		r.name = "Static";
		r.alns = Parameters::GetStaticAlnsIterations();
		r.driver_count = Parameters::GetDriverCount();
		r.time_horizon = Parameters::GetP();
		r.scenario_count = Parameters::GetScenarioCount();
		r.time_horizon_type = Parameters::GenerateIntelligentScenario()?1:0;
		r.idle_rate = report.GetIdleRate();
		return r;
	}
};


#endif
