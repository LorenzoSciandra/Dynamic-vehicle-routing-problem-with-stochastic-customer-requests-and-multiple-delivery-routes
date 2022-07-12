/*
	Branch-and-regret algorithm for solving SDDP
	On each event, we solve each scenario and look for
	decision that aren't making a consensus.
	There are 3 types of decision :
		1) A vehicle is departing now to deliver request i
		2) A vehicle is departing in the future to deliver request i
		3) Request i isn't deliered

	If there exists a request in the solution of the scenarios where
	there are more than two types of decision that are made, then
	We create impose those three decisions and calculate the cost of each one
	We implement the one with the smallest cost.
	This process is continued until there is a consensus among those types of decision.
*/

#ifndef BRANCH_REGRET_H
#define BRANCH_REGRET_H

#include "../Solution.h"
#include "Scenarios.h"
#include "DecisionMultiSet.h"
#include "../SolCompact.h"
#include "Report.h"
#include "Test.h"

class BranchRegretSimulation {
public:
    double cost;
    double time_taken;
    int unassigneds;
    double distance;
    int nb_routes;
    int nb_events;
    int unassigneds_oa; //customers that we accepted but couldn't deliver
    int nb_branchings; //number of branching performed in the branch-and-regret
    int nb_evaluated_nodes;

    int future_assignments; //for Ulmer instance
    int known_unassignments;//for Ulmer instance

    int _consensus_to_use;
    int _branching_to_use;
    bool _forbid_stochastic_drop_after_real;
    bool _allow_en_route_returns;

    SolCompact<Node, Driver> final_solution;
    Report report;

    BranchRegretSimulation() {}


    void Optimize(Scenarios &scenarios);

    double GetNextEvent(Scenarios &scenarios, Decisions &decs, double cur_time);

    void PerformOrderAcceptancy(std::vector <Prob<Node, Driver>> &probs, Decisions &decs, double cur_time,
                                DecisionMultiSet &multiset, Decisions &fixed_decisions);

    //return the number of customers that we accepted under the order acceptancy but that we couldn't deliver
    int GetUnassignCustomersOA(Decisions &decisions);

    void Show() {
        printf("BranchRegret ");
        printf("Cost:%.2lf Dist:%.2lf Un:%d Routes:%d ", cost, distance, unassigneds, nb_routes);
        printf("Time:%.2lf Events:%d branching:%d BR_nodes:%d ", time_taken, nb_events, nb_branchings,
               nb_evaluated_nodes);
        printf("Consensus:%d ", _consensus_to_use);
        printf("WaitAtDepot:%d ", (int) _forbid_stochastic_drop_after_real);
        printf("AllowDepotReturns:%d\n", (int) _allow_en_route_returns);
        if (unassigneds_oa != -1) printf("UnOA:%d", unassigneds_oa);
        printf("\n");
    }

    result GetResult() {
        result r;
        r.id = 9;
        r.nb_customers = Parameters::GetRealRequestCount();
        r.distance = distance;
        r.unassigneds = unassigneds;
        r.time_taken = time_taken;
        r.nb_routes = nb_routes;
        r.nb_events = nb_events;
        r.unassigneds_oa = unassigneds_oa;
        r.nb_branchings = nb_branchings;
        r.nb_evaluated_nodes = nb_evaluated_nodes;
        r.allow_pdr = _allow_en_route_returns;
        r.alns = Parameters::GetAlnsIterations();
        r.driver_count = Parameters::GetDriverCount();
        r.time_horizon = Parameters::GetP();
        r.scenario_count = Parameters::GetScenarioCount();
        r.time_horizon_type = Parameters::GenerateIntelligentScenario() ? 1 : 0;
        r.idle_rate = report.GetIdleRate();
        r.name = "BR";
        switch (_consensus_to_use) {
            case CONSENSUS_STACY:
                r.name.append("-RS");
                break;
            case CONSENSUS_BY_DRIVERS:
                r.name.append("-AS");
                break;
            case CONSENSUS_BY_MINIMAL_CHANGE:
                r.name.append("-ED");
                break;
            default:
                r.name.append("-Unknown");
        }
        switch (_branching_to_use) {
            case BRANCH_AND_REGRET_GONOW_WAIT:
                r.name.append("-GoNowWait");
                break;
            case BRANCH_AND_REGRET_ASSIGN_TO:
                r.name.append("-AssignTo");
                break;
            case BRANCH_AND_REGRET_ASSIGN_TO_AND_DONT:
                r.name.append("-AssignToAndDont");
                break;
            default:
                r.name.append("-Unknown");
        }
        return r;
    }

    void ShowReport() {
        report.Show();
    }
};

#endif
