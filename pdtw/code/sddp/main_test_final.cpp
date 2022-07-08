
/*
 * Copyright Jean-Francois Cote 2020
 *
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
 *
 *
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>



#include "Scenarios.h"
#include "Solver.h"
#include "BranchRegretSimulation.h"
#include "DynSimulation.h"
#include "StaticSimulation.h"
#include "ConsensusSimulation.h"
#include <vector>





void Execute(Scenarios & scenarios, config & conf);

int main(int arg, char ** argv)
{
	time_t tt = time(0);
	tt = 15646022;
	srand(tt);
	printf("Seed:%ld\n", tt);
	//srand(1304445903);

	Scenarios scenarios;
	if(arg > 1)
		scenarios.LoadStacyVoccia(argv[1]);
	else
	{
		printf("Provide an instance as parameter\n");
		exit(1);
	}

	std::vector<config> configs;
	for(int i=10;i<=12;i++)
		configs.push_back(config(250,i,9999,30));

	for(size_t i=0;i<configs.size();i++)
		Execute(scenarios, configs[i]);

	printf("File:%s\nInstance:%s Customers:%d\n", argv[1], scenarios.problem_name_tw.c_str(), scenarios.nb_real_requests_instance);
	for(size_t i=0;i<configs.size();i++)
		configs[i].Show();


	if(arg >= 3)
	{
		FILE * f = fopen(argv[2], "w");
		if(f != NULL)
		{
			fprintf(f,"%s;%s;%d;", argv[1], scenarios.problem_name_tw.c_str(),scenarios.nb_real_requests_instance);
			for(size_t i=0;i<configs.size();i++)
				configs[i].Print(f);
			fprintf(f,"\n");
			fclose(f);
		}
	}

	return 0;
}


void Execute(Scenarios & scenarios, config & conf)
{
	Parameters::SetAlnsIterations(conf.alns);
	Parameters::SetDriverCount(conf.driver_count);
	Parameters::SetScenarioCount(conf.scenario_count);
	Parameters::SetGenerateIntelligentScenario(false);
	Parameters::SetP(conf.time_horizon);
	printf("\n\nExecute AlnsIterations:%d Drivers:%d ", Parameters::GetAlnsIterations(), Parameters::GetDriverCount());
	printf("P:%d Scenarios:%d \n", Parameters::GetP(), Parameters::GetScenarioCount());
	{
	StaticSimulation sim;
	sim.Optimize(scenarios);
	conf.results.push_back( result(1,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}

	{
	DynSimulation sim;
	Parameters::SetAllowEnRouteDepotReturns(false);
	Parameters::SetOrderAcceptancy(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(2,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}

	{
	DynSimulation sim;
	Parameters::SetAllowEnRouteDepotReturns(true);
	Parameters::SetOrderAcceptancy(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(3,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}

	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	Parameters::SetAllowEnRouteDepotReturns(false);
	Parameters::SetForbidStochasticDropAfterReal(true);
	Parameters::SetOrderAcceptancy(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(4,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}
	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	Parameters::SetAllowEnRouteDepotReturns(false);
	Parameters::SetForbidStochasticDropAfterReal(false);
	Parameters::SetOrderAcceptancy(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(4,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}

	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	Parameters::SetAllowEnRouteDepotReturns(true);
	Parameters::SetForbidStochasticDropAfterReal(false);
	Parameters::SetOrderAcceptancy(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(5,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}


	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	Parameters::SetAllowEnRouteDepotReturns(true);
	Parameters::SetForbidStochasticDropAfterReal(false);
	Parameters::SetOrderAcceptancy(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(7,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}


	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
	Parameters::SetAllowEnRouteDepotReturns(true);
	Parameters::SetForbidStochasticDropAfterReal(false);
	Parameters::SetOrderAcceptancy(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(8,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}

	{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	Parameters::SetAllowEnRouteDepotReturns(true);
	Parameters::SetForbidStochasticDropAfterReal(false);
	Parameters::SetOrderAcceptancy(false);
	Parameters::SetEvaluateWaitingStrategy(true);
	Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_LEAST_COMMON);
	Parameters::SetGenerateIntelligentScenario(true);
	Parameters::SetP(0);
	sim.Optimize(scenarios);
	conf.results.push_back( result(9,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}
}
