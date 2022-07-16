
#include "ProgressiveSimulation.h"
#include "Decisions.h"
#include "DecisionMultiSet.h"
#include "Solver.h"
#include <time.h>
#include <limits.h>
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

    // ?
    Decision decision_wait;
    decision_wait.decision_type = DECISION_TYPE_GLOBAL;
    decision_wait.action_type = DECISION_ACTION_WAIT;

    Decisions prev_decisions;
    for (nb_events = 1; cur_time <= Parameters::GetTimeHorizon(); nb_events++) {
        Parameters::SetCurrentTime(cur_time);
        //printf("\n\n\nTime:%.1lf Unassigned:%d\n", cur_time, prev_decisions.GetUnassignedCount());
        //prev_decisions.Show();

        std::vector<Prob<Node, Driver>> probs;
        scenarios.Generate(probs, cur_time);
        DecisionMultiSet dec_progressive;

        // for all real request
        unsigned long j = 0;
        Decisions fixed_decisions; //principally acceptancy decisions
        while (j < scenarios.GetRealScenarioCustomerCount()) {
            for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
                Solver solver(probs[i]);
                solver.SetDecisions(prev_decisions);
                solver.SetDecisions(fixed_decisions);
                solver.Optimize();
                //printf("Scenario:%d Cost:%.1lf\n", i, solver.cost);
                //solver.Show();
                //solver.ShowCost();

                Decisions decisions;
                solver.GetDecisions(decisions);
                dec_progressive.Add(decisions);
                //solver.Optimize();
                //decisions.Show();
                //decisions.ShowByDriver(8);
            }

            std::vector<Decision> common_decisions = dec_progressive.GetDecisions(0)->GetRealDecisions();

            // Find the decisions that occur in all scenarios
            for (int i = 1; i < dec_progressive.GetReportCount(); ++i) {
                std::vector<Decision> cur_real = dec_progressive.GetDecisions(i)->GetRealDecisions();
                for (const auto &item: cur_real) {
                    // search for same node_id; if action type is not the same, remove.
                    bool item_corresponds = true;
                    int index = 0;
                    for (; index < common_decisions.size(); index++) {
                        Decision common_item = common_decisions.at(index);

                        if (item.node_id == common_item.node_id) {
                            if (item.action_type != common_item.action_type) {
                                item_corresponds = false;
                                break;
                            }
                        }
                    }

                    if (!item_corresponds) {
                        common_decisions.erase(common_decisions.begin() + index);
                    }
                }
            }

            j += common_decisions.size();
            fixed_decisions.AddAll(common_decisions);

            //dec_progressive.Show();
        }

        //cur_time = scenarios.GetNextEvent(cur_time);
        //prev_decisions.Show();
        cur_time = GetNextEvent(scenarios, prev_decisions, cur_time);
        //printf("Next Time:%.1lf \n", cur_time);

        prev_decisions.Remove(cur_time);
        //printf("\n\n");
    }

    _forbid_stochastic_drop_after_real = Parameters::ForbidStochasticDropAfterReal();
    _allow_en_route_returns = Parameters::AllowEnRouteDepotReturns();

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
    time_taken = chrono.getTime();
    Show();

    // to remove?
    if (!_forbid_stochastic_drop_after_real)
        final_solution.name.append(" WS");
    if (_allow_en_route_returns)
        final_solution.name.append(" PDR");
    solver.GetSolutionCompact(final_solution);
    Parameters::SetSBPA(false);
    solver.GetReport(report);
}


//	Types of event :
//	1) A new request arrived
//	2) A driver arrives at the depot
// 3) a driver has finished its waiting period at the depot
double ProgressiveSimulation::GetNextEvent(Scenarios &scenarios, Decisions &decs, double cur_time) {
    double next_request = scenarios.GetNextEvent(cur_time);
    double ne = decs.GetNextEvent();
    return std::min(ne, next_request);
}
