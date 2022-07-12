
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
#include "Test.h"

void Execute(Scenarios &scenarios, config &conf, std::vector <result> &results);

int main(int arg, char **argv) {
    time_t tt = time(0);
    tt = 15646022;
    srand(tt);
    printf("Seed:%ld\n", tt);
    //srand(1304445903);

    Scenarios scenarios;
    if (arg > 1)
        scenarios.LoadStacyVoccia(argv[1]);
    else {
        printf("Provide an instance as parameter\n");
        exit(1);
    }

    std::vector <config> configs;
    std::vector <result> results;
    configs.push_back(config(100, 10, 0, 30));

    for (size_t i = 0; i < configs.size(); i++)
        Execute(scenarios, configs[i], results);

    printf("File:%s\nInstance:%s Customers:%d\n", argv[1], scenarios.problem_name_tw.c_str(),
           scenarios.nb_real_requests_instance);
    for (size_t i = 0; i < results.size(); i++)
        results[i].Show();


    if (arg >= 3) {
        FILE *f = fopen(argv[2], "w");
        if (f != NULL) {
            for (size_t i = 0; i < results.size(); i++) {
                fprintf(f, "%s;%s;%d;", argv[1], scenarios.problem_name_tw.c_str(),
                        scenarios.nb_real_requests_instance);
                results[i].Print(f);
                fprintf(f, "\n");
            }
            fclose(f);
        }
    }

    return 0;
}


void Execute(Scenarios &scenarios, config &conf, std::vector <result> &results) {
    Parameters::SetAlnsIterations(conf.alns);
    Parameters::SetDriverCount(conf.driver_count);
    Parameters::SetScenarioCount(conf.scenario_count);
    Parameters::SetGenerateIntelligentScenario(true);
    Parameters::SetForbidStochasticDropAfterReal(false);
    Parameters::SetAllowEnRouteDepotReturns(false);
    Parameters::SetP(conf.time_horizon);
    int c = scenarios.nb_real_requests_instance;

    printf("\n\nExecute AlnsIterations:%d Drivers:%d ", Parameters::GetAlnsIterations(), Parameters::GetDriverCount());
    printf("P:%d Scenarios:%d \n", Parameters::GetP(), Parameters::GetScenarioCount());
    /*{
    StaticSimulation sim;
    sim.Optimize(scenarios);
    conf.results.push_back( result(1,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
    }*/


    {
        DynSimulation sim;
        sim.Optimize(scenarios);
        results.push_back(sim.GetResult());
    }


    {
        BranchRegretSimulation sim;
        Parameters::SetConsensusToUse(CONSENSUS_BY_DRIVERS);
        Parameters::SetBranchAndRegretToUse(BRANCH_AND_REGRET_GONOW_WAIT);
        Parameters::SetOrderAcceptancy(false);
        Parameters::SetEvaluateWaitingStrategy(true);
        Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
        sim.Optimize(scenarios);
        results.push_back(sim.GetResult());
    }

/*	{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	Parameters::SetBranchAndRegretToUse( BRANCH_AND_REGRET_ASSIGN_TO );
	Parameters::SetOrderAcceptancy(false);
	Parameters::SetEvaluateWaitingStrategy(true);
	Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
	sim.Optimize(scenarios);
	results.push_back(sim.GetResult());
}*/

    {
        BranchRegretSimulation sim;
        Parameters::SetConsensusToUse(CONSENSUS_BY_DRIVERS);
        Parameters::SetBranchAndRegretToUse(BRANCH_AND_REGRET_ASSIGN_TO_AND_DONT);
        Parameters::SetOrderAcceptancy(false);
        Parameters::SetEvaluateWaitingStrategy(true);
        Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
        sim.Optimize(scenarios);
        results.push_back(sim.GetResult());
    }
}
