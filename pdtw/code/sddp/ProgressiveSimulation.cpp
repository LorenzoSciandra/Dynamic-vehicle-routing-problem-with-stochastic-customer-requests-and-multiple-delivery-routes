
#include "ProgressiveSimulation.h"
#include "Decisions.h"
#include "DecisionMultiSet.h"
#include "Solver.h"
#include "Parameters.h"
#include "../../../lib/Chrono.h"
#include <stdlib.h>

void ProgressiveSimulation::Optimize(Scenarios &scenarios) {
    Parameters::SetProgressive(true);
    srand(Parameters::GetSeed());
    double cur_time = Parameters::GetTimeHorizonStartTime();
    {
        double t = scenarios.GetNextEvent(cur_time);
        if (t > cur_time) //there are no customer at the beginning
            cur_time = t;
    }
    printf("ProgressiveSimulation StartTime:%.1lf \n", cur_time);
    ChronoCpuNoStop chrono;
    chrono.start();

    Decisions prev_decisions;

    for (nb_events = 1; cur_time <= Parameters::GetTimeHorizon(); nb_events++) {
        Parameters::SetCurrentTime(cur_time);
        printf("\n\n\nTime:%.1lf Unassigned:%d\n", cur_time, prev_decisions.GetUnassignedCount());
//        prev_decisions.Show();

        std::vector<Prob<Node, Driver>> probs;
        scenarios.Generate(probs, cur_time);

        DecisionMultiSet BB_multiset;

        for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
            Solver solver(probs[i]);
            solver.SetDecisions(prev_decisions);
            solver.Optimize();
            Decisions decisions;
            solver.GetDecisions(decisions);
            BB_multiset.Add(decisions);
        }

        DecisionMultiSet best_integer_solution;

        BranchAndBound(BB_multiset, best_integer_solution, prev_decisions, scenarios, probs);

        switch (Parameters::GetConsensusToUse()) {
            case CONSENSUS_STACY:
                best_integer_solution.GetConsensusStacy(prev_decisions);
                break;
            case CONSENSUS_BY_DRIVERS:
                best_integer_solution.GetConsensusByDrivers(prev_decisions);
                break;
            case CONSENSUS_BY_MINIMAL_CHANGE:
                best_integer_solution.GetConsensusMinimalDistance(prev_decisions);
                break;
        }

//        prev_decisions.Show();

//        getchar();

        //new_dd.Show();

        cur_time = GetNextEvent(scenarios, prev_decisions, cur_time);
        //prev_decisions.Show();

        prev_decisions.Remove(cur_time);
        //printf("\n\n");

        //prev_decisions.Show();
        //new_dd.Show();

        //printf("Computation time:%.4lf\n", chrono.getTime()-chrono_cur_time);
        //getchar();

        if (chrono.getTime() > Parameters::GetTimeLimitSeconds()) {
            cost = 0;
            unassigneds = 0;
            distance = 0;
            nb_routes = 0;
            time_taken = 0;
            return;
        }
    }

    prev_decisions.Show();

    Prob<Node, Driver> real;
    scenarios.GenerateReal(real, Parameters::GetTimeHorizon() + 1);
    Parameters::SetCurrentTime(0);
    Solver solver(real);
    solver.SetDecisions(prev_decisions);
    //solver.Optimize();
    solver.Unfix();
    solver.Show();
    //prev_decisions.Show();

    solver.Update();
    cost = solver.cost;
    unassigneds = solver.nb_unassigneds / 2;
    distance = solver.distances;
    nb_routes = solver.GetRouteCount();
    time_taken = chrono.getTime();

//    _branching_to_use = Parameters::GetBranchAndRegretToUse();
//    _consensus_to_use = Parameters::GetConsensusToUse();
    _forbid_stochastic_drop_after_real = Parameters::ForbidStochasticDropAfterReal();
    _allow_en_route_returns = Parameters::AllowEnRouteDepotReturns();
    Show();

    switch (Parameters::GetConsensusToUse()) {
        case CONSENSUS_STACY:
            final_solution.name = "BranchAndRegret-RS";
            break;
        case CONSENSUS_BY_DRIVERS:
            final_solution.name = "BranchAndRegret-AS";
            break;
        case CONSENSUS_BY_MINIMAL_CHANGE:
            final_solution.name = "BranchAndRegret-ED";
            break;
    }
    if (_allow_en_route_returns)
        final_solution.name.append(" PDR");

    solver.GetSolutionCompact(final_solution);
    Parameters::SetBranchAndRegret(false);

    solver.GetReport(report);
    prev_decisions.GenerateGraph();
}

void ProgressiveSimulation::BranchAndBound(DecisionMultiSet &current_multiset,
                                           DecisionMultiSet &best_integer_solution,
                                           Decisions &working_decisions,
                                           Scenarios &scenarios,
                                           std::vector<Prob<Node, Driver>> &probs) {
    Decisions dd;

    current_multiset.GetNextActionDecisions(working_decisions, dd, scenarios);
    printf("dd.GetCount: %d\n", dd.GetCount());
//    getchar();
    if (dd.GetCount() == 0) {
        if (best_integer_solution.GetReportCount() == 0 || best_integer_solution.GetAverageCost() > current_multiset.GetAverageCost()) {
            printf("Updating integer: ");
            best_integer_solution = current_multiset;
        }
    } else {
        for (int i = 0; i < dd.GetCount(); i++) {
            Decisions curr = working_decisions;
            curr.Add(dd.Get(i));

            // Impostare una decisione -> Risolvere scenari -> recuperare ogni soluzione -> passiamo l'insieme di soluzioni al nodo successivo
            DecisionMultiSet BB_multiset;

            for (int j = 0; j < Parameters::GetScenarioCount(); j++) {
                Solver solver(probs[j]);
                solver.SetDecisions(curr);
                solver.Optimize();
                Decisions decisions;
                solver.GetDecisions(decisions);
                BB_multiset.Add(decisions);
            }

            BranchAndBound(BB_multiset, best_integer_solution, curr, scenarios, probs);
        }
    }
}

//	Types of event :
//	1) A new request arrived
//	2) A driver arrives at the depot
// 3) a driver has finished its waiting period at the depot
double ProgressiveSimulation::GetNextEvent(Scenarios &scenarios, Decisions &decs, double cur_time) {
    double next_request = scenarios.GetNextEvent(cur_time);
    double ne = decs.GetNextEvent(scenarios.real);

    return std::min(ne, next_request);
}
