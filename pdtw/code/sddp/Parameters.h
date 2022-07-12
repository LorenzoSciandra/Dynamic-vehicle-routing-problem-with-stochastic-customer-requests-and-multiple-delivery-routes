/*
 * Copyright Jean-Francois Cote 2016
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca
 * If you have improvements, please contact me!
 */
#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define CONSENSUS_STACY 1
#define CONSENSUS_BY_DRIVERS 2
#define CONSENSUS_BY_MINIMAL_CHANGE 3

#define BRANCH_AND_REGRET_GONOW_WAIT 1
#define BRANCH_AND_REGRET_ASSIGN_TO 2
#define BRANCH_AND_REGRET_ASSIGN_TO_AND_DONT 3

#define INSTANCE_TYPE_STACY 1
#define INSTANCE_TYPE_ULMER 2

class Parameters {

public:

    static int GetAlnsIterations() { return _alns_iterations; }

    static int GetStaticAlnsIterations() { return _static_alns_iterations; }

    static double GetTimeHorizon() { return _time_horizon; }

    static double GetTimeHorizonStartTime() { return _start_time; }

    static int GetDriverCount() { return _driver_count; }

    static double GetCurrentTime() { return _current_time; }

    static int GetP() { return p; }

    static int GetP2() { return p2; }

    static bool IsDebug() { return _debug; }

    static bool IsStacyInstance() { return is_stacy_instance; }

    static double GetRealRequestWeight() { return real_request_weight; }

    static bool MoveLatestPickupFirst() { return _move_latest_pickup_first; }

    static bool IncludeUnassignmentDecision() { return _include_unassignment_decision; }

    static bool AssignToAllDrivers() { return _assign_to_all_drivers; }

    static int GetConsensusToUse() { return _consensus_to_use; }

    static int GetBranchAndRegretToUse() { return _branch_and_regret_to_use; }

    static bool AllowEnRouteDepotReturns() { return _allow_en_route_return; }

    static bool ForbidStochasticDropAfterReal() { return _forbid_stochastic_drop_after_real; }

    static bool EvaluateRejectOnlyIfNot0() { return _evaluate_reject_if_not_0; }

    static int GetWaitingTime() { return _waiting_time; }

    static int GetScenarioCount() { return _scenario_count; }

    static int GetTimeLimitSeconds() { return _time_limit_seconds; }

    static bool PerformOrderAcceptancy() { return _perform_order_acceptancy; }

    static bool MinimizeWaiting() { return _minimize_waiting; }

    static bool GenerateIntelligentScenario() { return _gen_intelligent_scenarios; }

    static int GetDecisionSelectionMode() { return _decision_selection_mode; }

    static bool EvaluateWaitingStrategy() { return _evaluate_wait_strategy; }

    static int GetSeed() { return _seed; }

    static int GetUlmerDepotLocation() { return _ulmer_depot_location; }

    static int GetInstanceType() { return _instance_type; }

    static bool IsBranchAndRegret() { return _is_branch_and_regret; }

    static bool IsSBPA() { return _is_sbpa; }

    static int GetRealRequestCount() { return _nb_real_requests; }

    static void SetAlnsIterations(int it) { _alns_iterations = it; }

    static void SetStaticAlnsIterations(int it) { _static_alns_iterations = it; }

    static void SetTimeHorizon(double time) { _time_horizon = time; }

    static void SetTimeHorizonStartTime(double st) { _start_time = st; }

    static void SetDriverCount(int cn) { _driver_count = cn; }

    static void SetCurrentTime(double tt) { _current_time = tt; }

    static void SetRealRequestWeight(double i) { real_request_weight = i; }

    static void SetStacyInstance(bool i) { is_stacy_instance = i; }

    static void SetConsensusToUse(int c) { _consensus_to_use = c; }

    static void SetBranchAndRegretToUse(int c) { _branch_and_regret_to_use = c; }

    static void SetAllowEnRouteDepotReturns(bool b) { _allow_en_route_return = b; }

    static void SetForbidStochasticDropAfterReal(bool b) { _forbid_stochastic_drop_after_real = b; }

    static void SetAssignToAllDrivers(bool v) { _assign_to_all_drivers = v; }

    static void SetIncludeUnassignementDecision(bool v) { _include_unassignment_decision = v; }

    static void SetEvaluateRejectOnlyIfNot0(bool v) { _evaluate_reject_if_not_0 = v; }

    static void SetP(int i) { p = i; }

    static void SetP2(int i) { p2 = i; }

    static void SetWaitingTime(int v) { _waiting_time = v; }

    static void SetDebug(bool b) { _debug = b; }

    static void SetScenarioCount(int sc) { _scenario_count = sc; }

    static void SetOrderAcceptancy(bool b) { _perform_order_acceptancy = b; }

    static void SetMinimizeWaiting(bool b) { _minimize_waiting = b; }

    static void SetGenerateIntelligentScenario(bool b) { _gen_intelligent_scenarios = b; }

    static void SetDecisionSelectionMode(int d) { _decision_selection_mode = d; }

    static void SetEvaluateWaitingStrategy(bool b) { _evaluate_wait_strategy = b; }

    static void SetSeed(int se) { _seed = se; }

    static void SetUlmerDepotLocation(int i) { _ulmer_depot_location = i; }

    static void SetInstanceType(int t) { _instance_type = t; }

    static void SetBranchAndRegret(bool b) {
        _is_branch_and_regret = b;
        if (b) _is_sbpa = false;
    }

    static void SetSBPA(bool b) {
        _is_sbpa = b;
        if (b) _is_branch_and_regret = false;
    }

    static void SetRealRequestCount(int r) { _nb_real_requests = r; }

private:

    static int _alns_iterations;
    static int _static_alns_iterations; //alns iterations for the static case
    static double _time_horizon;
    static int _driver_count;
    static int _scenario_count;
    static int _time_limit_seconds;
    static double _current_time;
    static double real_request_weight;
    static bool is_stacy_instance;

    static bool _is_branch_and_regret;
    static bool _is_sbpa;


    static int p;
    static int p2;
    static double _start_time; //start time of the time horizon
    static bool _assign_to_all_drivers;    //boolean indicating if a decision for each driver is generated in DecisionMultiSet
    static bool _move_latest_pickup_first;
    static bool _include_unassignment_decision;

    static bool _allow_en_route_return; //allow a en route driver to return to the depot to do other pickups before continueing its route
    static bool _forbid_stochastic_drop_after_real;

    static int _waiting_time;

    static int _consensus_to_use;
    static int _branch_and_regret_to_use;

    static bool _debug;
    static bool _perform_order_acceptancy;

    static bool _minimize_waiting;

    static bool _gen_intelligent_scenarios;

    static int _decision_selection_mode;

    static bool _evaluate_wait_strategy;//once all the scenarios have been optimized, at the beginning of the b&r,
    //evaluate the cost of waiting for free request
    //if the cost is almost the same as the previous cost, wait all requests

    static bool _evaluate_reject_if_not_0; //evaluate the reject option if it is present in at least 1 scenario
    static int _seed;
    static int _ulmer_depot_location; //1 is (5000,5000), 2 is (0,10000) and 3 is (0,0)
    static int _instance_type; //1 is stacy, 2 is ulmer

    static int _nb_real_requests;
};


#endif
