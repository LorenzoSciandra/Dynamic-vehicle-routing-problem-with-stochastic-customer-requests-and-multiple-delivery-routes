
#include "BranchRegretSimulation.h"
#include "Decisions.h"
#include "DecisionMultiSet.h"
#include "Solver.h"
#include <time.h>
#include <limits.h>
#include "Parameters.h"
#include "../../../lib/Chrono.h"
#include <stdlib.h>


void BranchRegretSimulation::Optimize(Scenarios &scenarios) {
    Parameters::SetBranchAndRegret(true);
    srand(Parameters::GetSeed());
    double prev_time = 0;
    nb_branchings = nb_evaluated_nodes = 0;
    double cur_time = Parameters::GetTimeHorizonStartTime();
    {
        double t = scenarios.GetNextEvent(cur_time);
        if (t > cur_time) //there are no customer at the beginning
            cur_time = t;
    }

    printf("BranchRegret StartTime:%.1lf P:%d Alns:%d OA:%d Sc:%d BR:%d\n", cur_time, Parameters::GetP(),
           Parameters::GetAlnsIterations(), (int) Parameters::PerformOrderAcceptancy(), scenarios.GetScenarioCount(),
           Parameters::GetBranchAndRegretToUse());
    ChronoCpuNoStop chrono;
    chrono.start();

    Decision decision_wait;
    decision_wait.decision_type = DECISION_TYPE_GLOBAL;
    decision_wait.action_type = DECISION_ACTION_WAIT;

    int skipped = 0;
    Decisions fixed_decisions; //principally acceptancy decisions
    Decisions prev_decisions;
    for (nb_events = 1; cur_time <= Parameters::GetTimeHorizon(); nb_events++) {
        double chrono_cur_time = chrono.getTime();

        Parameters::SetCurrentTime(cur_time);
        int current_un_count = prev_decisions.GetUnassignedCount();
        //printf("\n\n\nTime:%.1lf Unassigned:%d HasDriverAtDepot:%d\n", cur_time, current_un_count, (int)prev_decisions.HasDriverAtDepot());
        //prev_decisions.Show();

        std::vector<Prob<Node, Driver>> probs;
        scenarios.Generate(probs, cur_time);

        DecisionMultiSet multiset;

        if (Parameters::PerformOrderAcceptancy())
            PerformOrderAcceptancy(probs, prev_decisions, cur_time, multiset, fixed_decisions);

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
                multiset.Add(decisions);

                if (Parameters::EvaluateWaitingStrategy()) {
                    int wait = solver.EvaluateAllWait();
                    min_waiting_time = std::min(min_waiting_time, wait);
                    solver.GetDecisions(decisions);
                    multiset_wait.Add(decisions);
                }
            }

            //if(Parameters::EvaluateWaitingStrategy())
            //	printf("Initial Average:%.1lf AverageWait:%.1lf MinWait:%d\n", multiset.GetAverageCost(), multiset_wait.GetAverageCost(), min_waiting_time);

            if (Parameters::EvaluateWaitingStrategy() &&
                multiset_wait.GetAverageCost() <= 1.001 * multiset.GetAverageCost()) {
                multiset = multiset_wait;
                wait_to_next_event = true;

                decision_wait.arrival_time = cur_time;
                decision_wait.departure_time = cur_time + min_waiting_time;
            }
        } else {
            skipped++;
        }

        //multiset.Show();

        Decisions new_dd;
        Decisions working_dd = prev_decisions;
        Decisions dd;

        switch (Parameters::GetBranchAndRegretToUse()) {
            case BRANCH_AND_REGRET_GONOW_WAIT:
                multiset.GetNextActionDecisions(working_dd, dd, scenarios);
                break;
            case BRANCH_AND_REGRET_ASSIGN_TO:
                multiset.GetNextAssignmentDecisions(working_dd, dd, scenarios);
                break;
            case BRANCH_AND_REGRET_ASSIGN_TO_AND_DONT:
                multiset.GetNextAssignmentDecisions2(working_dd, dd, scenarios);
                break;
        }

        DecisionMultiSet best_set = multiset;
        if (!wait_to_next_event)
            while (dd.GetCount() > 0) {
                nb_branchings++;
                //dd.Show();
                //printf("Time:%.1lf ddCount:%d\n",cur_time,dd.GetCount() );
                int cheapest_dec = -1;
                double cheapest_avg = BIG_DOUBLE;
                double max_real_un_count = 999;
                DecisionMultiSet prev_set = best_set;
                for (int i = 0; i < dd.GetCount(); i++) {
                    Decisions cur = working_dd;
                    cur.Add(dd.Get(i));
                    nb_evaluated_nodes++;
                    //dd.Get(i)->Show();

                    //no need to calculate the cost of unassigning the request if we could serve them all
                    if (max_real_un_count <= current_un_count + 0.01 &&
                        dd.Get(i)->decision_type == DECISION_TYPE_ASSIGNMENT && dd.Get(i)->driver_id == -1)
                        continue;

                    DecisionMultiSet temp_set;
                    for (int j = 0; j < Parameters::GetScenarioCount(); j++) {
                        if (prev_set.GetDecisions(j)->HasDecisionCharacteristics(dd.Get(i))) {
                            //printf("Scenario:%d\n", j);
                            //prev_set.GetDecisions(j)->ShowOnlyForRequest(dd.Get(i)->req_id);
                            temp_set.Add(prev_set.GetDecisions(j));
                            continue;
                        }
                        Solver solver(probs[j]);
                        solver.SetDecisions(cur);
                        solver.Optimize();

                        //printf("\t\tScenario:%d Cost:%.1lf Un:%d RealUn:%d\n", j, solver.cost, solver.nb_unassigneds, solver.nb_real_unassigneds);
                        Decisions decisions;
                        solver.GetDecisions(decisions);

                        if (!decisions.HasDecisionCharacteristics(dd.Get(i)) &&
                            !decisions.HasDecisionReqDriver(dd.Get(i)->req_id, -1)) {
                            printf("Inconherent solution Time:%.1lf\n", Parameters::GetCurrentTime());
                            dd.Get(i)->Show();
                            decisions.ShowOnlyForRequest(dd.Get(i)->req_id);

                            solver.Show();
                            exit(1);
                        }
                        temp_set.Add(decisions);
                    }
                    max_real_un_count = std::min(temp_set.GetAverageRealUn(), max_real_un_count);
                    //
                    //printf("\tReq:%d Decision:%d AvgCost:%.1lf RealUn:%.3lf ",dd.Get(i)->req_id, i, temp_set.GetAverageCost(),temp_set.GetAverageRealUn()); dd.Get(i)->Show();

                    if (cheapest_avg > temp_set.GetAverageCost()) {
                        cheapest_avg = temp_set.GetAverageCost();
                        best_set = temp_set;
                        cheapest_dec = i;
                    }
                }

                int cd = cheapest_dec;
                int pr_id = dd.Get(cd)->req_id;
                //printf("Time:%.1lf Req:%d BestDec:%d BestCost:%.1lf ",cur_time,dd.Get(cd)->req_id, cd, cheapest_avg); dd.Get(cd)->Show();

                new_dd.Add(dd.Get(cheapest_dec));
                working_dd.Add(dd.Get(cheapest_dec));
                switch (Parameters::GetBranchAndRegretToUse()) {
                    case BRANCH_AND_REGRET_GONOW_WAIT:
                        best_set.GetNextActionDecisions(working_dd, dd, scenarios);
                        break;
                    case BRANCH_AND_REGRET_ASSIGN_TO:
                        best_set.GetNextAssignmentDecisions(working_dd, dd, scenarios);
                        break;
                    case BRANCH_AND_REGRET_ASSIGN_TO_AND_DONT:
                        best_set.GetNextAssignmentDecisions2(working_dd, dd, scenarios);
                        break;
                }
                //dd.Show();
            }

        switch (Parameters::GetConsensusToUse()) {
            case CONSENSUS_STACY:
                best_set.GetConsensusStacy(prev_decisions);
                break;
            case CONSENSUS_BY_DRIVERS:
                best_set.GetConsensusByDrivers(prev_decisions);
                break;
            case CONSENSUS_BY_MINIMAL_CHANGE:
                best_set.GetConsensusMinimalDistance(prev_decisions);
                break;
        }

        if (decision_wait.arrival_time == cur_time)
            prev_decisions.Add(decision_wait);

        //new_dd.Show();

        prev_time = cur_time;
        cur_time = GetNextEvent(scenarios, prev_decisions, cur_time);
        //prev_decisions.Show();

        prev_decisions.Remove(cur_time);
        prev_decisions.Add(fixed_decisions);
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

        //if(cur_time >= 95.1) exit(1);
    }

    //prev_decisions.Show();

    Prob<Node, Driver> real;
    scenarios.GenerateReal(real, Parameters::GetTimeHorizon() + 1);
    Parameters::SetCurrentTime(0);
    Solver solver(real);
    solver.SetDecisions(prev_decisions);
    //solver.Optimize();
    solver.Unfix();
    //solver.Show();
    //prev_decisions.Show();

    solver.Update();
    cost = solver.cost;
    unassigneds = solver.nb_unassigneds / 2;
    distance = solver.distances;
    nb_routes = solver.GetRouteCount();
    unassigneds_oa = Parameters::PerformOrderAcceptancy() ? GetUnassignCustomersOA(fixed_decisions) : -1;
    time_taken = chrono.getTime();

    future_assignments = solver.GetFutureCustomerAssignmentCount();
    known_unassignments = solver.GetInitialCustomerUnassignmentCount();

    _branching_to_use = Parameters::GetBranchAndRegretToUse();
    _consensus_to_use = Parameters::GetConsensusToUse();
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


//	Types of event :
//	1) A new request arrived and there is a vehicle at the depot
//	2) A driver arrives at the depot
// 3) a driver has finished its waiting period at the depot
double BranchRegretSimulation::GetNextEvent(Scenarios &scenarios, Decisions &decs, double cur_time) {
    double next_request = scenarios.GetNextEvent(cur_time);
    double ne = decs.GetNextEvent(scenarios.real);
    return std::min(ne, next_request);
}

void BranchRegretSimulation::PerformOrderAcceptancy(std::vector<Prob<Node, Driver>> &probs, Decisions &decs,
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

int BranchRegretSimulation::GetUnassignCustomersOA(Decisions &decisions) {
    std::map<int, bool> accepted_orders;
    for (int i = 0; i < decisions.GetCount(); i++) {
        Decision *d = decisions.Get(i);
        if (d->node_type == NODE_TYPE_PICKUP && d->decision_type == DECISION_TYPE_ACTION &&
            d->action_type == DECISION_ACTION_DONT_UNASSIGN)
            accepted_orders[d->req_id] = true;
    }

    int nb_un_oa = 0;
    for (int i = 0; i < decisions.GetCount(); i++) {
        Decision *d = decisions.Get(i);
        if (d->node_type == NODE_TYPE_PICKUP && d->decision_type == DECISION_TYPE_ASSIGNMENT && d->driver_id == -1 &&
            accepted_orders[d->req_id])
            nb_un_oa++;
    }
    return nb_un_oa;
}
