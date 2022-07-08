
/*
 * Copyright Jean-Francois Cote 2019
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 *
 * If you have improvements, please contact me!
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
#include "Test.h"

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
	configs.push_back(config(100,10,0,5));
	configs.push_back(config(100,10,0,10));
	configs.push_back(config(100,10,0,20));
	configs.push_back(config(100,10,0,30));

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
	Parameters::SetGenerateIntelligentScenario(true);

	Parameters::SetAlnsIterations(conf.alns);
	Parameters::SetDriverCount(conf.driver_count);
	Parameters::SetP(conf.time_horizon);
	Parameters::SetScenarioCount(conf.scenario_count);
	int c = scenarios.nb_real_requests_instance;


	Parameters::SetOrderAcceptancy(false);					//for branch-and-regret
	Parameters::SetEvaluateWaitingStrategy(true);		//for branch-and-regret
	Parameters::SetEvaluateRejectOnlyIfNot0(false);		//for branch-and-regret
	Parameters::SetAllowEnRouteDepotReturns(false);
	Parameters::SetForbidStochasticDropAfterReal(false);

	printf("\n\nExecute AlnsIterations:%d Drivers:%d ", Parameters::GetAlnsIterations(), Parameters::GetDriverCount());
	printf("P:%d Scenarios:%d \n", Parameters::GetP(), Parameters::GetScenarioCount());

/*	{
	StaticSimulation sim;
	sim.Optimize(scenarios);
	conf.results.push_back( result(1,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}


	{
	DynSimulation sim;
	Parameters::SetAllowEnRouteDepotReturns(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(3,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
}*/


	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	sim.Optimize(scenarios);
	conf.results.push_back( result(6,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}

	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	sim.Optimize(scenarios);
	conf.results.push_back( result(7,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}

	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
	sim.Optimize(scenarios);
	conf.results.push_back( result(8,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}

	{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
	sim.Optimize(scenarios);
	conf.results.push_back( result(9,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	conf.results.back().nb_branchings = sim.nb_branchings;
	}

	{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
	sim.Optimize(scenarios);
	conf.results.push_back( result(9,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	conf.results.back().nb_branchings = sim.nb_branchings;
	}

	{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
	Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
	sim.Optimize(scenarios);
	conf.results.push_back( result(9,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	conf.results.back().nb_branchings = sim.nb_branchings;
	}


}
