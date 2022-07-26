
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

    Decisions prev_decisions;
    for (nb_events = 1; cur_time <= Parameters::GetTimeHorizon(); nb_events++) {
        Parameters::SetCurrentTime(cur_time);
        printf("\n\n\nTime:%.1lf Unassigned:%d\n", cur_time, prev_decisions.GetUnassignedCount());
        prev_decisions.Show();

        std::vector<Prob<Node, Driver>> probs;
        scenarios.Generate(probs, cur_time);
        DecisionMultiSet dec_progressive;

        for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
            Solver solver(probs[i]);
            solver.SetDecisions(prev_decisions);
            solver.Optimize();
            Decisions decisions;
            solver.GetDecisions(decisions);
            dec_progressive.Add(decisions);
        }

        Decisions fixed_decisions;
        Decision fixed_decision;

        bool has_next = dec_progressive.GetNextDecisionProgressive(fixed_decisions, fixed_decision);
        while (has_next) {
            fixed_decisions.Add(fixed_decision);
            DecisionMultiSet multiset;
            for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
                Solver solver(probs[i]);
                Decisions dd = prev_decisions;
                dd.Add(fixed_decisions);
                solver.SetDecisions(dd);
                solver.Optimize();
                Decisions decisions;
                solver.GetDecisions(decisions);
                multiset.Add(decisions);
            }

            has_next = multiset.GetNextDecisionProgressive(fixed_decisions, fixed_decision);
        }
        cur_time = GetNextEvent(scenarios, prev_decisions, cur_time);

        prev_decisions.Remove(cur_time);
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

DecisionMultiSet ProgressiveSimulation::BranchAndBound(Scenarios &scenarios) {
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

    DecisionMultiSet solution;

    Decisions prev_decisions;
    for (nb_events = 1; cur_time <= Parameters::GetTimeHorizon(); nb_events++) {
        Parameters::SetCurrentTime(cur_time);
        printf("\n\n\nTime:%.1lf Unassigned:%d\n", cur_time, prev_decisions.GetUnassignedCount());
        prev_decisions.Show();

        std::vector<Prob<Node, Driver>> probs;
        scenarios.Generate(probs, cur_time);
        DecisionMultiSet dec_progressive;

        for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
            Solver solver(probs[i]);
            solver.SetDecisions(prev_decisions);
            solver.Optimize();
            Decisions decisions;
            solver.GetDecisions(decisions);
            dec_progressive.Add(decisions);
        }

        Decisions fixed_decisions;

        solution = RecursiveExploration(probs, dec_progressive, fixed_decisions, solution);
        
        cur_time = GetNextEvent(scenarios, prev_decisions, cur_time);

        prev_decisions.Remove(cur_time);
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
    return solution;
}

DecisionMultiSet ProgressiveSimulation::RecursiveExploration(vector<Prob<Node, Driver>> &probs, DecisionMultiSet &multiset, Decisions &fixed_decisions, DecisionMultiSet &solution){

    Decision fixed_decision;

    bool has_next = multiset.GetNextDecisionProgressive(fixed_decisions, fixed_decision);

    if(!has_next){
        return solution;
    }
    else{

        Decision go_now = fixed_decision;
        Decision wait = fixed_decision;
        Decision dont_deliver = fixed_decision;

        go_now.action_type = DECISION_ACTION_GO_NOW;
        wait.action_type = DECISION_ACTION_WAIT;
        dont_deliver.action_type = DECISION_ACTION_DONT_DELIVER;

        DecisionMultiSet go_now_solution;
        DecisionMultiSet wait_solution;
        DecisionMultiSet dont_deliver_solution;

        for(int j=0; j < 3; j++){
            if(j == 0){
                fixed_decisions.Add(go_now);
                solution.Add(fixed_decisions);
            }
            else if(j == 1){
                fixed_decisions.Add(wait);
                solution.Add(fixed_decisions);
            }
            else if(j == 2){
                dont_deliver_decisions.Add(dont_deliver);
                solution.Add(fixed_decisions);
            }
            DecisionMultiSet multiset;
            for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
                Solver solver(probs[i]);
                solver.SetDecisions(fixed_decisions);
                solver.Optimize();
                Decisions decisions;
                solver.GetDecisions(decisions);
                multiset.Add(decisions);
            }
            if(j == 0){
                go_now_solution = RecursiveExploration(probs, multiset, fixed_decisions, solution);
                fixed_decisions.Remove(go_now);
            }
            else if(j == 1){
                wait_solution = RecursiveExploration(probs, multiset, fixed_decisions, solution);
                fixed_decisions.Remove(wait);
            }
            else if(j == 2){
                dont_deliver_solution = RecursiveExploration(probs, multiset, fixed_decisions, solution);
                fixed_decisions.Remove(dont_deliver);
            }
        }
        
        if (go_now_solution.GetAverageCost() < wait_solution.GetAverageCost()){
            if (go_now_solution.GetAverageCost() < dont_deliver_solution.GetAverageCost()){
                return go_now_solution;
            }
            else{
                return dont_deliver_solution;
            }
        }
        else{
            if (wait_solution.GetAverageCost() < dont_deliver_solution.GetAverageCost()){
                return wait_solution;
            }
            else{
                return dont_deliver_solution;
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
    double ne = decs.GetNextEvent();
    return std::min(ne, next_request);
}
