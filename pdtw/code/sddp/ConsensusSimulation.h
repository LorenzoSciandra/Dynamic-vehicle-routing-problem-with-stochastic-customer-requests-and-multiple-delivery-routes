#ifndef CONSENSUS_SIMULATION_H
#define CONSENSUS_SIMULATION_H

#include "Scenarios.h"
#include "Decisions.h"
#include "Parameters.h"
#include "../SolCompact.h"
#include "Report.h"
#include "Test.h"

class ConsensusSimulation
{
public:

	double cost;
	double time_taken;
	int unassigneds;
	double distance;
	int nb_routes;
	int nb_events;

	int _consensus_to_use;
	bool _forbid_stochastic_drop_after_real;
	bool _allow_en_route_returns;
	SolCompact<Node,Driver> final_solution;
	Report report;
	ConsensusSimulation(){}


	void Optimize(Scenarios & scenarios);

	double GetNextEvent(Scenarios & scenarios, Decisions & decs, double cur_time);

	void Show()
	{
		printf("Consensus ");
		printf("Cost:%.2lf Dist:%.2lf Un:%d Routes:%d ", cost, distance, unassigneds, nb_routes);
		printf("Time:%.2lf Events:%d ", time_taken,nb_events);
		printf("Consensus:%d ", _consensus_to_use);
		printf("WaitAtDepot:%d ", (int)_forbid_stochastic_drop_after_real);
		printf("AllowDepotReturns:%d\n", (int)_allow_en_route_returns);
	}

	void ShowReport()
	{
		report.Show();
	}

	result GetResult()
	{
		result r;
		if(_consensus_to_use == CONSENSUS_STACY)
			r.id = 6;
		else if(_consensus_to_use == CONSENSUS_BY_DRIVERS)
			r.id = 7;
		else if(_consensus_to_use == CONSENSUS_BY_MINIMAL_CHANGE)
			r.id = 8;
		else
			r.name = "SBPA-Unknown";

		r.nb_customers = Parameters::GetRealRequestCount();
		r.distance = distance;
		r.unassigneds = unassigneds;
		r.time_taken = time_taken;
		r.nb_routes = nb_routes;
		r.nb_events = nb_events;
		r.allow_pdr = _allow_en_route_returns;
		r.alns = Parameters::GetAlnsIterations();
		r.driver_count = Parameters::GetDriverCount();
		r.time_horizon = Parameters::GetP();
		r.scenario_count = Parameters::GetScenarioCount();
		r.time_horizon_type = Parameters::GenerateIntelligentScenario()?1:0;
		r.idle_rate = report.GetIdleRate();
		
		if(_consensus_to_use == CONSENSUS_STACY)
			r.name = "SBPA-RS";
		else if(_consensus_to_use == CONSENSUS_BY_DRIVERS)
			r.name = "SBPA-AS";
		else if(_consensus_to_use == CONSENSUS_BY_MINIMAL_CHANGE)
			r.name = "SBPA-ED";
		else
			r.name = "SBPA-Unknown";
		return r;
	}
};


#endif
