
/*
 * Copyright Jean-Francois Cote 2012
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at cotejean@iro.umontreal.ca for questions
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
#include "ProgressiveSimulation.h"
#include "DynSimulation.h"
#include "StaticSimulation.h"
#include "ConsensusSimulation.h"

/**
 * @brief  Main function for the SDDP algorithm.
 * 
 * @param arg 
 * @param argv  
 * @return int 
 */


int main(int arg, char **argv) {
    time_t tt = time(0);
    tt = 15646022;
    srand(tt);
    printf("Seed:%ld\n", tt);
    //srand(1304445903);

    Parameters::SetAlnsIterations(100);
    Parameters::SetDriverCount(10);
    Parameters::SetP(0);
    Parameters::SetScenarioCount(30);
    Parameters::SetGenerateIntelligentScenario(true);


    Scenarios scenarios_ulmer;
    Scenarios scenarios;

    if (arg > 1) {
        scenarios.LoadStacyVoccia(argv[1]);
        //scenarios_ulmer.LoadUlmer(argv[1]);
        //scenarios_ulmer.GenerateUlmerScenarios(scenarios, 0);
        //scenarios.Show();
    } else {
        printf("Provide an instance as parameter\n");
        exit(1);
    }
    printf("File:%s\nInstance:%s\n", argv[1], scenarios.problem_name_tw.c_str());
    printf("AlnsIterations:%d Drivers:%d ", Parameters::GetAlnsIterations(), Parameters::GetDriverCount());
    printf("P:%d Scenarios:%d \n", Parameters::GetP(), Parameters::GetScenarioCount());

    /*Prob<Node,Driver> pr;
    scenarios.GenerateReal(pr, 9999999);
    pr.Show();
    pr.ShowMatrices();*/

    std::vector <result> results;

    StaticSimulation ssim;
    //ssim.Optimize(scenarios);

    ProgressiveSimulation ps;
    Parameters::SetAllowEnRouteDepotReturns(false);
    Parameters::SetForbidStochasticDropAfterReal(true);
    ps.Optimize(scenarios);
    results.push_back(ps.GetResult());

    printf("File:%s\nInstance:%s Customers:%d\n", argv[1], scenarios.problem_name_tw.c_str(),
           scenarios.nb_real_requests_instance);
    for (auto & result : results)
        result.Show();

/*
    DynSimulation dsim1;
    Parameters::SetAllowEnRouteDepotReturns(false);
    dsim1.Optimize(scenarios);
    results.push_back(dsim1.GetResult());


    DynSimulation dsim2;
    Parameters::SetAllowEnRouteDepotReturns(true);
    dsim2.Optimize(scenarios);
    results.push_back(dsim2.GetResult());

    ConsensusSimulation con1;
    Parameters::SetConsensusToUse(CONSENSUS_STACY);
    Parameters::SetAllowEnRouteDepotReturns(false);
    Parameters::SetForbidStochasticDropAfterReal(true);
    con1.Optimize(scenarios);
    results.push_back(con1.GetResult());

    ConsensusSimulation con2;
    Parameters::SetConsensusToUse(CONSENSUS_BY_MINIMAL_CHANGE);
    Parameters::SetAllowEnRouteDepotReturns(false);
    Parameters::SetForbidStochasticDropAfterReal(true);
    con2.Optimize(scenarios);
    results.push_back(con2.GetResult());

    ConsensusSimulation con3;
    Parameters::SetConsensusToUse(CONSENSUS_BY_DRIVERS);
    Parameters::SetAllowEnRouteDepotReturns(false);
    Parameters::SetForbidStochasticDropAfterReal(true);
    con3.Optimize(scenarios);
    results.push_back(con3.GetResult());


    BranchRegretSimulation br1;
    Parameters::SetConsensusToUse(CONSENSUS_STACY);
    Parameters::SetAllowEnRouteDepotReturns(false);
    Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
    Parameters::SetForbidStochasticDropAfterReal(false);
    Parameters::SetOrderAcceptancy(false);
    br1.Optimize(scenarios);
    results.push_back(br1.GetResult());

    BranchRegretSimulation br2;
    Parameters::SetConsensusToUse(CONSENSUS_BY_DRIVERS);
    Parameters::SetAllowEnRouteDepotReturns(false);
    Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
    Parameters::SetForbidStochasticDropAfterReal(false);
    Parameters::SetOrderAcceptancy(false);
    br2.Optimize(scenarios);
    results.push_back(br2.GetResult());


    BranchRegretSimulation br3;
    Parameters::SetConsensusToUse(CONSENSUS_BY_MINIMAL_CHANGE);
    Parameters::SetAllowEnRouteDepotReturns(false);
    Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
    Parameters::SetForbidStochasticDropAfterReal(false);
    Parameters::SetOrderAcceptancy(false);
    br3.Optimize(scenarios);
    results.push_back(br3.GetResult());

    printf("File:%s\nInstance:%s Customers:%d\n", argv[1], scenarios.problem_name_tw.c_str(),
           scenarios.nb_real_requests_instance);
    for (size_t i = 0; i < results.size(); i++)
        results[i].Show();
*/

    /*
    printf("%%Offline\n\\addplot[color=black, solid] ");
    ssim.ShowReport();
    printf("%%Reoptimization\n\\addplot[color=orange, solid] ");
    dsim1.ShowReport();
    printf("%%SBPA-Stacy\n\\addplot[color=magenta, solid] ");
    printf("%%");
    con1.ShowReport();
    printf("%%B&R-AS\n\\addplot[color=blue, solid] ");
    br2.ShowReport();
    */
    /*
    ConsensusSimulation con1;
    Parameters::SetConsensusToUse( CONSENSUS_STACY );
    Parameters::SetAllowEnRouteDepotReturns(false);
    Parameters::SetForbidStochasticDropAfterReal(true);
    //con1.Optimize(scenarios);

    ConsensusSimulation con2;
    Parameters::SetConsensusToUse( CONSENSUS_STACY );
    Parameters::SetAllowEnRouteDepotReturns(false);
    Parameters::SetForbidStochasticDropAfterReal(false);
    //con2.Optimize(scenarios);

    ConsensusSimulation con3;
    Parameters::SetConsensusToUse( CONSENSUS_STACY );
    Parameters::SetAllowEnRouteDepotReturns(true);
    Parameters::SetForbidStochasticDropAfterReal(false);
    //con3.Optimize(scenarios);

    ConsensusSimulation con4;
    Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
    Parameters::SetAllowEnRouteDepotReturns(true);
    Parameters::SetForbidStochasticDropAfterReal(false);
    //con4.Optimize(scenarios);

    ConsensusSimulation con5;
    Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
    Parameters::SetAllowEnRouteDepotReturns(true);
    Parameters::SetForbidStochasticDropAfterReal(false);
    con5.Optimize(scenarios);


    BranchRegretSimulation br;
    Parameters::SetEvaluateWaitingStrategy(true);
    Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
    Parameters::SetAllowEnRouteDepotReturns(true);
    Parameters::SetForbidStochasticDropAfterReal(false);
    Parameters::SetOrderAcceptancy(false);
    br.Optimize(scenarios);

    BranchRegretSimulation br2;
    Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
    Parameters::SetAllowEnRouteDepotReturns(true);
    Parameters::SetForbidStochasticDropAfterReal(false);
    Parameters::SetOrderAcceptancy(true);
    //br2.Optimize(scenarios);


    printf("\n\n\n");
    printf("File:%s\nInstance:%s Customers:%d\n", argv[1], scenarios.problem_name_tw.c_str(), scenarios.GetRealScenarioCustomerCount());
    printf("AlnsIterations:%d Drivers:%d ", Parameters::GetAlnsIterations(), Parameters::GetDriverCount());
    printf("P:%d Scenarios:%d \n", Parameters::GetP(), Parameters::GetScenarioCount());
    //ssim.Show();
    //dsim1.Show();
    //dsim2.Show();
    //con1.Show();
    //con2.Show();
    //con3.Show();
    //con4.Show();
    con5.Show();
    br.Show();
    //br2.Show();



    if(arg >= 3)
    {
        FILE * f = fopen(argv[2], "w");
        fprintf(f,"%s;%s;%d;", argv[1], scenarios.problem_name_tw.c_str(),scenarios.GetRealScenarioCustomerCount());
        fprintf(f,"%d;%d;", Parameters::GetAlnsIterations(), Parameters::GetDriverCount());
        fprintf(f,"%d;%d;", Parameters::GetP(), Parameters::GetScenarioCount());
        fprintf(f,"%.2lf;%d;%.2lf;%d;", ssim.distance,ssim.unassigneds, ssim.time_taken, ssim.nb_routes);
        fprintf(f,"%.2lf;%d;%.2lf;%d;%d;", dsim1.distance,dsim1.unassigneds, dsim1.time_taken,dsim1.nb_routes,dsim1.nb_events);
        fprintf(f,"%.2lf;%d;%.2lf;%d;%d;", dsim2.distance,dsim2.unassigneds, dsim2.time_taken,dsim2.nb_routes,dsim2.nb_events);
        fprintf(f,"%.2lf;%d;%.2lf;%d;%d;", con1.distance,con1.unassigneds, con1.time_taken,con1.nb_routes,con1.nb_events);
        fprintf(f,"%.2lf;%d;%.2lf;%d;%d;", con2.distance,con2.unassigneds, con2.time_taken,con2.nb_routes,con2.nb_events);
        fprintf(f,"%.2lf;%d;%.2lf;%d;%d;", con3.distance,con3.unassigneds, con3.time_taken,con3.nb_routes,con3.nb_events);
        fprintf(f,"%.2lf;%d;%.2lf;%d;%d;", con4.distance,con4.unassigneds, con4.time_taken,con4.nb_routes,con4.nb_events);
        fprintf(f,"%.2lf;%d;%.2lf;%d;%d;", con5.distance,con5.unassigneds, con5.time_taken,con5.nb_routes,con5.nb_events);
        fprintf(f,"%.2lf;%d;%.2lf;%d;%d;", br.distance,br.unassigneds, br.time_taken,br.nb_routes,br.nb_events);
        fprintf(f,"%.2lf;%d;%.2lf;%d;%d;%d;", br2.distance,br2.unassigneds, br2.time_taken,br2.nb_routes,br2.nb_events, br2.unassigneds_oa);
        fprintf(f,"\n");
    }
    */
    return 0;
}
