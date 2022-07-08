
/*
 * Copyright Jean-Francois Cote 2019
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
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


void Execute(Scenarios & scenarios, config & conf, std::vector<result> & results);

int main(int arg, char ** argv)
{
	time_t tt = time(0);
	tt = 15646022;
	srand(tt);
	printf("Seed:%ld\n", tt);
	//srand(1304445903);

	Scenarios scenarios;
	if(arg >= 3)
		scenarios.LoadStacyVoccia(argv[1]);
	else
	{
		printf("Provide the config file\n");
		printf("Provide the number of drivers\n");
		printf("Provide the output file\n");
		exit(1);
	}

	int nb_drivers = 0;
	sscanf(argv[2], "%d", &nb_drivers);

	std::vector<result> results;
	std::vector<config> configs;
	configs.push_back(config(100,nb_drivers,0,30));
	//configs.push_back(config(100,8,0,30));
	//configs.push_back(config(100,10,0,30));
	//configs.push_back(config(100,12,0,30));
	//configs.push_back(config(100,14,0,30));

	for(size_t i=0;i<configs.size();i++)
		Execute(scenarios, configs[i], results);

	printf("File:%s\nInstance:%s Customers:%d\n", argv[1], scenarios.problem_name_tw.c_str(), scenarios.nb_real_requests_instance);
	for(size_t i=0;i<results.size();i++)
		results[i].Show();


	if(arg >= 3)
	{
		FILE * f = fopen(argv[3], "w");
		if(f != NULL)
		{
			for(size_t i=0;i<results.size();i++)
			{
				fprintf(f,"%s;%s;%d;", argv[1], scenarios.problem_name_tw.c_str(),scenarios.nb_real_requests_instance);
				results[i].Print(f);
				fprintf(f,"\n");
			}
			fclose(f);
		}
	}

	return 0;
}


void Execute(Scenarios & scenarios, config & conf, std::vector<result> & results)
{
	Parameters::SetAlnsIterations(conf.alns);
	Parameters::SetDriverCount(conf.driver_count);
	Parameters::SetP(conf.time_horizon);
	Parameters::SetScenarioCount(conf.scenario_count);
	int c = scenarios.nb_real_requests_instance;

	Parameters::SetGenerateIntelligentScenario(true);
	Parameters::SetOrderAcceptancy(false);					//for branch-and-regret
	Parameters::SetEvaluateWaitingStrategy(true);		//for branch-and-regret
	Parameters::SetEvaluateRejectOnlyIfNot0(false);		//for branch-and-regret
	Parameters::SetForbidStochasticDropAfterReal(false);
	Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON); //for branch-and-regret

	printf("\n\nExecute AlnsIterations:%d Drivers:%d ", Parameters::GetAlnsIterations(), Parameters::GetDriverCount());
	printf("P:%d Scenarios:%d \n", Parameters::GetP(), Parameters::GetScenarioCount());


	Parameters::SetAllowEnRouteDepotReturns(false);

	{
	DynSimulation sim;
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}

	/*{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}*/

	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}

	/*{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}*/

	/*{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
}*/

	{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}

	/*{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}*/





	Parameters::SetAllowEnRouteDepotReturns(true);
	{
	DynSimulation sim;
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}

	/*{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}*/

	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}

	/*{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}*/

	/*{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}*/

	{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}

	/*{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
	}*/

}
