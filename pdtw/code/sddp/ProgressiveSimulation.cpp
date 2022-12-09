
#include "ProgressiveSimulation.h"
#include "Decisions.h"
#include "DecisionMultiSet.h"
#include "Solver.h"
#include "Parameters.h"
#include "../../../lib/Chrono.h"
#include "BBNode.h"
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
        Decisions best_decisions;

        BBNodes.clear();
        BBNode node;
        BBNodes.push_back(node);

        BranchAndBound(BB_multiset, best_integer_solution, prev_decisions, scenarios, probs, best_decisions, node);

        PreprocessBBNodes(best_integer_solution.GetAverageCost());

        BBNode_total_count += BBNodes.size();

        PrintBBNodes(cur_time, best_integer_solution.GetAverageCost());

        prev_decisions = best_decisions;

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

        cur_time = GetNextEvent(scenarios, prev_decisions, cur_time);

        prev_decisions.Remove(cur_time);

        if (chrono.getTime() > Parameters::GetTimeLimitSeconds()) {
            cost = 0;
            unassigneds = 0;
            distance = 0;
            nb_routes = 0;
            time_taken = 0;
            return;
        }
    }

    Prob<Node, Driver> real;
    scenarios.GenerateReal(real, Parameters::GetTimeHorizon() + 1);
    Parameters::SetCurrentTime(0);
    Solver solver(real);
    solver.SetDecisions(prev_decisions);
    solver.Unfix();
    solver.Show();

    solver.Update();
    cost = solver.cost;
    unassigneds = solver.nb_unassigneds / 2;
    distance = solver.distances;
    nb_routes = solver.GetRouteCount();
    time_taken = chrono.getTime();

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
}

void ProgressiveSimulation::BranchAndBound(DecisionMultiSet &current_multiset, DecisionMultiSet &best_integer_solution,
                                           Decisions &working_decisions, Scenarios &scenarios,
                                           std::vector<Prob<Node, Driver>> &probs,
                                           Decisions &best_decisions, BBNode &node) {
    Decisions dd;

    current_multiset.GetNextActionDecisions(working_decisions, dd, scenarios);
    if (dd.GetCount() == 0) {
        if (best_integer_solution.GetReportCount() == 0 ||
            best_integer_solution.GetAverageCost() > current_multiset.GetAverageCost()) {
            best_integer_solution = current_multiset;
            best_decisions = working_decisions;
        }
    } else {
        for (int i = 0; i < dd.GetCount(); i++) {
            Decisions curr = working_decisions;
            curr.Add(dd.Get(i));

            DecisionMultiSet BB_multiset;

            for (int j = 0; j < Parameters::GetScenarioCount(); j++) {
                Solver solver(probs[j]);
                solver.SetDecisions(curr);
                solver.Optimize();
                Decisions decisions;
                solver.GetDecisions(decisions);
                BB_multiset.Add(decisions);
            }

            BBNode new_node;
            new_node.id = BBNodes.size();
            new_node.tree_level = node.tree_level + 1;
            // Added to have a back reference to the children
            new_node.parent_id = node.id;
            new_node.decision_type = dd.Get(i)->action_type;
            new_node.request_id = dd.Get(i)->req_id;
            new_node.cost = BB_multiset.GetAverageCost();
            BBNodes.push_back(new_node);

            for (BBNode &item: BBNodes) {
                if (item.id == node.id)
                    item.children.push_back(new_node);
            }

//            node.children.push_back(new_node);

            // Fathoming
            if (!Parameters::IsFathomingInBnBEnabled()) {
                BranchAndBound(BB_multiset, best_integer_solution, curr, scenarios, probs, best_decisions, new_node);
            } else if ((best_integer_solution.GetReportCount() == 0 ||
                        best_integer_solution.GetAverageCost() > BB_multiset.GetAverageCost())) {
                BranchAndBound(BB_multiset, best_integer_solution, curr, scenarios, probs, best_decisions, new_node);
            }
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

void ProgressiveSimulation::PreprocessBBNodes(double best_integer_solution_cost) {
    for (auto &item: BBNodes) {
        if (item.children.empty() && std::abs(item.cost - best_integer_solution_cost) < DBL_EPSILON) {
            item.edge_best = true;

            int index = item.parent_id;
            while (index != -1) {
                BBNodes.at(index).edge_best = true;
                index = BBNodes.at(index).parent_id;
            }
        }

        if (!item.children.empty() && (item.edge_regret || item.id == -1)) {
            BBNode &best = item.children.at(0);
            // find the best node that would be used by B&R
            for (BBNode &item2: item.children) {
                if (item.cost < best.cost) {
                    best = item2;
                }
            }

            BBNodes.at(best.id).edge_regret = true;
        }
    }
}

void ProgressiveSimulation::PrintBBNodes(double time, double best_integer_solution_avg_cost) {
    if (BBNodes.empty()) {
        return;
    }

    printf("\ndigraph G%d {\nlabelloc=\"t\";\n", BBnodesPrintCount);
    printf("-1 [label=\"Depot\\nTime: %.0lf\\nBest: %lf\"]\n", time, best_integer_solution_avg_cost);
    for (BBNode &item: BBNodes) {
        item.DeclareNode();
    }

    for (BBNode &item: BBNodes) {
        item.ToGraph();
    }

    printf("subgraph structs {\n"
           "    node [shape=plaintext]\n"
           "    struct1 [label=<\n"
           "        <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n"
           "    <tr>\n"
           "    <td colspan=\"2\">Legend</td>\n"
           "  </tr>\n"
           "            <tr>\n"
           "    <td>Algorithm</td>\n"
           "    <td>Color</td>\n"
           "  </tr>\n"
           "  <tr>\n"
           "    <td>Branch &amp; Regret</td>\n"
           "    <td BGCOLOR=\"red\"></td>\n"
           "  </tr>\n"
           "  <tr>\n"
           "    <td>Best Path</td>\n"
           "    <td BGCOLOR=\"blue\"></td>\n"
           "  </tr>\n"
           "        </TABLE>>];\n"
           "}");

    printf("}\n\n");
    BBnodesPrintCount++;
}
