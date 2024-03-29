#include "BranchAndBoundSimulation.h"
#include "Decisions.h"
#include "DecisionMultiSet.h"
#include "Solver.h"
#include "Parameters.h"
#include "../../../lib/Chrono.h"
#include "BBNode.h"
#include <stdlib.h>
#include <queue>
#include <algorithm>

void BranchAndBoundSimulation::Optimize(Scenarios &scenarios) {
    Parameters::SetProgressive(true);
    srand(Parameters::GetSeed());
    double cur_time = Parameters::GetTimeHorizonStartTime();
    {
        double t = scenarios.GetNextEvent(cur_time);
        if (t > cur_time) //there are no customer at the beginning
            cur_time = t;
    }
    printf("BranchAndBoundSimulation StartTime:%.1lf \n", cur_time);
    ChronoCpuNoStop chrono;
    chrono.start();

    int skipped = 0;

    Decisions prev_decisions;

    for (nb_events = 1; cur_time <= Parameters::GetTimeHorizon(); nb_events++) {
        Parameters::SetCurrentTime(cur_time);

        std::vector<Prob<Node, Driver>> probs;
        scenarios.Generate(probs, cur_time);

        DecisionMultiSet BB_multiset;

        if (Parameters::PerformOrderAcceptancy())
            PerformOrderAcceptancy(probs, prev_decisions, cur_time, BB_multiset, prev_decisions);

        bool wait_to_next_event = false;

        //check if we can arrive inside the time window of a real request, if we can't we go the next event
        bool cant_wait_to_serve_a_request = true;
        for (int i = 0; i < probs[0].GetRequestCount(); i++)
            if (probs[0].GetRequest(i)->GetNode(0)->is_real && !scenarios.CanWait(probs[0].GetRequest(i)->GetNode(1))) {
                cant_wait_to_serve_a_request = false;
                break;
            }
        if (cant_wait_to_serve_a_request)
            wait_to_next_event = true;

        //Evaluate the cost of waiting
        if (!wait_to_next_event) {
            DecisionMultiSet multiset_wait;
            int min_waiting_time = Parameters::GetTimeHorizon();
            for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
                Solver solver(probs[i]);
                solver.SetDecisions(prev_decisions);
                solver.Optimize();
                //printf("Scenario:%d Cost:%.1lf\n", i, solver.cost);
                Decisions decisions;
                solver.GetDecisions(decisions);
                BB_multiset.Add(decisions);

                if (Parameters::EvaluateWaitingStrategy()) {
                    int wait = solver.EvaluateAllWait();
                    min_waiting_time = std::min(min_waiting_time, wait);
                    solver.GetDecisions(decisions);
                    multiset_wait.Add(decisions);
                }
            }

//            if (Parameters::EvaluateWaitingStrategy())
//                printf("Initial Average:%.1lf AverageWait:%.1lf MinWait:%d\n", BB_multiset.GetAverageCost(),
//                       multiset_wait.GetAverageCost(), min_waiting_time);

            if (Parameters::EvaluateWaitingStrategy() &&
                multiset_wait.GetAverageCost() <= 1.001 * BB_multiset.GetAverageCost()) {
                BB_multiset = multiset_wait;
                wait_to_next_event = true;
            }
        }


        if (!wait_to_next_event) {
            Decisions best_decisions;

            DecisionMultiSet best_integer_solution;

            ResetBB();

            BBNode *node = new BBNode();
            node->tree_level = 0;

            BBNodes.push_back(node);

            // printf("\nInitial cost: %.1lf, Wait: %s", BB_multiset.GetAverageCost(), wait_to_next_event?"true":"false");

            if(Parameters::ShouldUseIterativeBranchAndBound()) {
                IterativeBranchAndBound(BB_multiset, best_integer_solution, prev_decisions, scenarios,
                                        probs, best_decisions, node);
            }
            else {
                BranchAndBound(BB_multiset, best_integer_solution, prev_decisions, scenarios,
                               probs, best_decisions, node);
            }

            BBNode_total_count += BBNodes.size();

            if (Parameters::ShouldPrintBBTrees()) {
                PreprocessBBNodes(best_integer_solution.GetAverageCost());
                PrintBBNodes(cur_time, best_integer_solution.GetAverageCost());
            }

            BB_multiset = best_integer_solution;
            prev_decisions = best_decisions;


        } else {
            skipped++;
        }

        switch (Parameters::GetConsensusToUse()) {
            case CONSENSUS_STACY:
                BB_multiset.GetConsensusStacy(prev_decisions);
                break;
            case CONSENSUS_BY_DRIVERS:
                BB_multiset.GetConsensusByDrivers(prev_decisions);
                break;
            case CONSENSUS_BY_MINIMAL_CHANGE:
                BB_multiset.GetConsensusMinimalDistance(prev_decisions);
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
    printf("Skipped: %d\n", skipped);
}

void BranchAndBoundSimulation::PerformOrderAcceptancy(std::vector<Prob<Node, Driver>> &probs, Decisions &decs,
                                                      double cur_time, DecisionMultiSet &multiset,
                                                      Decisions &fixed_decisions) {
    //printf("PerformOrderAcceptancy curtime:%.2lf\n", cur_time);
    std::vector<int> new_requests;
    for (int i = 0; i < probs[0].GetCustomerCount(); i++) {
        Node *n = probs[0].GetCustomer(i);
        if (n->type != NODE_TYPE_PICKUP) continue;
        if (!n->is_real) continue;
        if (n->release_time < cur_time - 0.001) continue;
        if (n->release_time > cur_time + 0.001) continue;

        new_requests.push_back(n->req_id);
    }
    if (new_requests.size() == 0) return;

    //printf("Requests:");
    //for(size_t r=0;r<new_requests.size();r++)
    //	printf("%d ",new_requests[r]);
    //printf("\n");

    for (size_t r = 0; r < new_requests.size(); r++) {
        Decision d1;
        d1.req_id = new_requests[r];
        d1.decision_type = DECISION_TYPE_ACTION;
        d1.action_type = DECISION_ACTION_DONT_UNASSIGN;
        Decisions decs1 = decs;
        decs1.Add(d1);

        double cost_assign = 0;
        int sum_real_un = 0;
        DecisionMultiSet multiset1;
        for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
            Solver solver(probs[i]);
            solver.SetDecisions(decs1);
            solver.Optimize();
            cost_assign += solver.cost / Parameters::GetScenarioCount();
            sum_real_un += solver.nb_real_unassigneds;

            if (r + 1 == new_requests.size()) {
                Decisions decisions;
                solver.GetDecisions(decisions);
                multiset1.Add(decisions);
            }
        }


        Decision d2;
        d2.req_id = new_requests[r];
        d2.decision_type = DECISION_TYPE_ASSIGNMENT;
        d2.driver_id = -1;
        Decisions decs2 = decs;
        decs2.Add(d2);

        double cost_unassign = 0;
        DecisionMultiSet multiset2;
        if (sum_real_un >= 1)
            for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
                Solver solver(probs[i]);
                solver.SetDecisions(decs2);
                solver.Optimize();
                cost_unassign += solver.cost / Parameters::GetScenarioCount();

                if (r + 1 == new_requests.size()) {
                    Decisions decisions;
                    solver.GetDecisions(decisions);
                    multiset2.Add(decisions);
                }
            }

        //printf("Order to accept req:%d Assign:%.3lf SumRealUn:%d Unassign:%.3lf -> Assign\n",new_requests[r], cost_assign,sum_real_un, cost_unassign);

        if (cost_assign < cost_unassign || sum_real_un == 0) {
            decs.Add(d1);
            fixed_decisions.Add(d1);
            if (r + 1 == new_requests.size())
                multiset = multiset1;
        } else {
            decs.Add(d2);
            fixed_decisions.Add(d2);
            if (r + 1 == new_requests.size())
                multiset = multiset2;
        }

    }

}

void BranchAndBoundSimulation::BranchAndBound(DecisionMultiSet &current_multiset,
                                              DecisionMultiSet &best_integer_solution,
                                              Decisions &working_decisions,
                                              Scenarios &scenarios,
                                              std::vector<Prob<Node, Driver>> &probs,
                                              Decisions &best_decisions,
                                              BBNode *node) {
    Decisions current_decisions;

    current_multiset.GetNextActionDecisions(working_decisions, current_decisions, scenarios);
    if (current_decisions.GetCount() == 0) {
        if (best_integer_solution.GetReportCount() == 0 ||
            best_integer_solution.GetAverageCost() > current_multiset.GetAverageCost()) {
            best_integer_solution = current_multiset;
            best_decisions = working_decisions;
        }
    } else {
        std::vector<BBBestPriorityItem> all_decisions;

        for (int i = 0; i < current_decisions.GetCount(); i++) {
            Decisions curr = working_decisions;
            curr.Add(current_decisions.Get(i));

            DecisionMultiSet BB_multiset;

            for (int j = 0; j < Parameters::GetScenarioCount(); j++) {
                Solver solver(probs[j]);
                solver.SetDecisions(curr);
                solver.Optimize();
                Decisions decisions;
                solver.GetDecisions(decisions);
                BB_multiset.Add(decisions);
            }

            BBNode *new_node = new BBNode();
            new_node->id = BBNodes.size();
            new_node->tree_level = node->tree_level + 1;
            // Added to have a back reference to the children
            new_node->parent_id = node->id;
            new_node->decision_type = current_decisions.Get(i)->action_type;
            new_node->request_id = current_decisions.Get(i)->req_id;
            new_node->cost = BB_multiset.GetAverageCost();
            BBNodes.push_back(new_node);

            all_decisions.emplace_back(BB_multiset, curr, new_node);

            for (BBNode *item: BBNodes) {
                if (item->id == node->id)
                    item->children.push_back(new_node);
            }
        }

        if (Parameters::IsBestFirstUsedInBnB())
            std::sort(all_decisions.begin(), all_decisions.end(), &BBBestPriorityItem::comparator);

        // Fathoming
        for (BBBestPriorityItem &item: all_decisions) {
            item.bbNode->visit_order = visit_order_counter++;

            if (!Parameters::IsFathomingInBnBEnabled() ||
                (best_integer_solution.GetReportCount() == 0 ||
                 best_integer_solution.GetAverageCost() > item.value)) {
                BranchAndBound(item.multiSet, best_integer_solution, item.decisions, scenarios, probs,
                               best_decisions,
                               item.bbNode);
            }
        }
    }
}

void BranchAndBoundSimulation::IterativeBranchAndBound(DecisionMultiSet &current_multiset,
                                                       DecisionMultiSet &best_integer_solution,
                                                       Decisions &working_decisions,
                                                       Scenarios &scenarios,
                                                       std::vector<Prob<Node, Driver>> &probs,
                                                       Decisions &best_decisions,
                                                       BBNode *node) {

    std::vector<BBBestPriorityItem> all_decisions;

    do {
        Decisions current_decisions;
        current_multiset.GetNextActionDecisions(working_decisions, current_decisions, scenarios);

        if (!all_decisions.empty()) {
            BBBestPriorityItem &nextDecision = all_decisions.at(all_decisions.size()-1);
            nextDecision.bbNode->visit_order = visit_order_counter++;
            current_multiset = nextDecision.multiSet;
            working_decisions = nextDecision.decisions;
            node = nextDecision.bbNode;

            all_decisions.pop_back();
        }

        if (best_integer_solution.GetReportCount() == 0 ||
            best_integer_solution.GetAverageCost() > current_multiset.GetAverageCost()) {

            if (current_decisions.GetCount() == 0) {
                best_integer_solution = current_multiset;
                best_decisions = working_decisions;
                //            std::printf("Terminato nodo %ld, numero nodi: %ld\n", node->id, all_decisions.size());
            }
            else{
                for (int i = 0; i < current_decisions.GetCount(); i++) {
                    Decisions curr = working_decisions;
                    curr.Add(current_decisions.Get(i));

                    DecisionMultiSet BB_multiset;

                    for (int j = 0; j < Parameters::GetScenarioCount(); j++) {
                        Solver solver(probs[j]);
                        solver.SetDecisions(curr);
                        solver.Optimize();
                        Decisions decisions;
                        solver.GetDecisions(decisions);
                        BB_multiset.Add(decisions);
                    }

                    BBNode *new_node = new BBNode();
                    new_node->id = BBNodes.size();
                    new_node->tree_level = node->tree_level + 1;
                    //Added to have a back reference to the children
                    new_node->parent_id = node->id;
                    new_node->decision_type = current_decisions.Get(i)->action_type;
                    new_node->request_id = current_decisions.Get(i)->req_id;
                    new_node->cost = BB_multiset.GetAverageCost();
                    BBNodes.push_back(new_node);

                    all_decisions.emplace_back(BB_multiset, curr, new_node);

                    for (BBNode *item: BBNodes) {
                        if (item->id == node->id)
                            item->children.push_back(new_node);
                    }
                }

                if (Parameters::IsBestFirstUsedInBnB())
                    std::sort(all_decisions.begin(), all_decisions.end(), &BBBestPriorityItem::comparator);
            }
        }
    } while (!all_decisions.empty());
}


//	Types of event :
//	1) A new request arrived
//	2) A driver arrives at the depot
// 3) a driver has finished its waiting period at the depot
double BranchAndBoundSimulation::GetNextEvent(Scenarios &scenarios, Decisions &decs, double cur_time) {
    double next_request = scenarios.GetNextEvent(cur_time);
    double ne = decs.GetNextEvent(scenarios.real);

    return std::min(ne, next_request);
}

void BranchAndBoundSimulation::PreprocessBBNodes(double best_integer_solution_cost) {
    for (auto &item: BBNodes) {
        // Best Path
        if (item->children.empty() && std::abs(item->cost - best_integer_solution_cost) < DBL_EPSILON) {
            item->edge_best = true;

            long index = item->parent_id;
            while (index != -1) {
                BBNodes.at(index)->edge_best = true;
                index = BBNodes.at(index)->parent_id;
            }
        }


        // B&R Path; run only when the current BBNode is the depot
        if (item->id == -1) {
            BBNode *item2 = item;

            while (!item2->children.empty()) {
                BBNode *best = item2->children.at(0);

                for (BBNode *item3: item2->children) {
                    if (item3->cost < best->cost) {
                        best = item3;
                    }
                }
                BBNodes.at(best->id)->edge_regret = true;
                item2 = best;
            }
        }
    }
}

void BranchAndBoundSimulation::PrintBBNodes(double time, double best_integer_solution_avg_cost) {
    if (BBNodes.empty()) {
        return;
    }

    printf("%s", Parameters::PrintCurrentElapsedTime().c_str());
    printf("\ndigraph G%d {\nlabelloc=\"t\";\n", BBnodesPrintCount);
    printf("-1 [label=\"Depot\\nTime: %.0lf\\nBest: %.1lf\"]\n", time, best_integer_solution_avg_cost);
    for (BBNode *item: BBNodes) {
        item->DeclareNode();
    }

    for (BBNode *item: BBNodes) {
        item->ToGraph();
    }

    printf("subgraph structs {\n"
           "    node [shape=plaintext]\n"
           "    struct1 [label=<\n"
           "        <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n"
           "    <tr>\n"
           "    <td colspan=\"2\">Edge Legend</td>\n"
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
           "    <td>%s Best Path</td>\n"
           "    <td BGCOLOR=\"blue\"></td>\n"
           "  </tr>\n"
           "        </TABLE>>];\n"
           "struct2 [label=<\n"
           "        <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n"
           "    <tr>\n"
           "    <td colspan=\"2\">Node Legend</td>\n"
           "  </tr>\n"
           "  <tr>\n"
           "    <td>Decision Type</td>\n"
           "    <td>Color</td>\n"
           "  </tr>\n"
           "  <tr>\n"
           "    <td>Go Now</td>\n"
           "    <td BGCOLOR=\"green\"></td>\n"
           "  </tr>\n"
           "  <tr>\n"
           "    <td>Wait</td>\n"
           "    <td BGCOLOR=\"orange\"></td>\n"
           "  </tr>\n"
           "  <tr>\n"
           "    <td>Don't deliver</td>\n"
           "    <td BGCOLOR=\"red\"></td>\n"
           "  </tr>\n"
           "        </TABLE>>];\n"
           "}", Parameters::IsBestFirstUsedInBnB() ? "BF " : "DF ");

    printf("}\n\n");
    fflush(stdout);
    BBnodesPrintCount++;
}

void BranchAndBoundSimulation::ResetBB() {
    BBNodes.clear();
    visit_order_counter = 0;
}

BBBestPriorityItem::BBBestPriorityItem(
        const DecisionMultiSet &multiSet,
        const Decisions &decisions,
        BBNode *bbNode)
        : multiSet(multiSet), decisions(decisions), bbNode(bbNode) {
    value = this->multiSet.GetAverageCost();
}

bool BBBestPriorityItem::comparator(const BBBestPriorityItem &item1, const BBBestPriorityItem &item2) {
    return item1.value > item2.value;
}