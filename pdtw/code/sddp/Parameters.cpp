#include "Parameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string>
#include <string.h>
#include <vector>
#include <limits>
#include "Decision.h"

int Parameters::_static_alns_iterations = 2500; 
int Parameters::_alns_iterations = 50;
double Parameters::_time_horizon = -1;
int Parameters::_driver_count = 10;
double Parameters::_current_time = 0;
double Parameters::real_request_weight = 1;
bool Parameters::is_stacy_instance = false;
int Parameters::p = 9999;
double Parameters::_start_time = 0;
bool Parameters::_assign_to_all_drivers = false;
bool Parameters::_move_latest_pickup_first = true;
bool Parameters::_include_unassignment_decision = true;
int Parameters::_seed = 0;
int Parameters::_instance_type = 1;
int Parameters::_nb_real_requests = -1;

bool Parameters::_forbid_stochastic_drop_after_real = true;
bool Parameters::_allow_en_route_return = false;
int Parameters::_consensus_to_use = CONSENSUS_STACY;
int Parameters::_branch_and_regret_to_use = BRANCH_AND_REGRET_GONOW_WAIT;
bool Parameters::_perform_order_acceptancy = false;

bool Parameters::_debug = false;

int Parameters::_waiting_time = 1;
int Parameters::_scenario_count = 30;
int Parameters::_time_limit_seconds = 3600 * 24;
bool Parameters::_minimize_waiting = true;

bool Parameters::_gen_intelligent_scenarios = false;

int Parameters::_decision_selection_mode = DECISION_SELECTION_MODE_MOST_COMMON;

bool Parameters::_evaluate_wait_strategy = true;
bool Parameters::_evaluate_reject_if_not_0 = true;

int Parameters::_ulmer_depot_location = 1;

bool Parameters::_is_branch_and_regret = false;
bool Parameters::_is_sbpa = false;
