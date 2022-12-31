
#include "DynSimulation.h"
#include "Decisions.h"
#include "DecisionMultiSet.h"
#include "Solver.h"
#include <time.h>
#include <limits.h>
#include "Parameters.h"
#include "../../../lib/Chrono.h"
#include <stdlib.h>


void DynSimulation::Optimize(Scenarios &scenarios) {
    srand(Parameters::GetSeed()); // seed the random number generator
    nb_events = 0; // number of events in the real scenario
    double cur_time = Parameters::GetTimeHorizonStartTime(); // current time in the real scenario
    _forbid_stochastic_drop_after_real = Parameters::ForbidStochasticDropAfterReal(); // forbid stochastic drop after real requests
    _allow_en_route_returns = Parameters::AllowEnRouteDepotReturns(); // allow en route returns

    {
        double t = scenarios.GetNextEvent(cur_time); // get the next event in the real scenario
        if (t > cur_time) //there are no customer at the beginning
            cur_time = t; // set the current time to the next event
    }
    printf("Dynamic StartTime:%.1lf \n", cur_time); // print the start time of the real scenario
    ChronoCpuNoStop chrono; 
    chrono.start(); // start the chrono

    Decisions prev_decisions;
    for (nb_events = 0; cur_time <= Parameters::GetTimeHorizon(); nb_events++) {
        Parameters::SetCurrentTime(cur_time); // set the current time
        printf("\n\n\nTime:%.1lf Unassigned:%d\n", cur_time, prev_decisions.GetUnassignedCount());
        prev_decisions.Show();

        // create the current problem as default class
        Prob<Node, Driver> prob; 
        scenarios.GenerateReal(prob, cur_time); // initialize the current problem with the real scenario and stochastic scenarios

        Solver solver(prob); // create the solver
        solver.SetDecisions(prev_decisions); // set the previous decisions
        solver.Optimize(); // optimize the problem
        //solver.Show();
        //printf("Time:%d Dist:%.2lf Un:%d\n", (int)cur_time, solver.distances, solver.nb_unassigneds/2);

        Decisions decisions; // create the decisions
        solver.GetDecisions(decisions); // get the decisions


        //cur_time = scenarios.GetNextEvent(cur_time);
        cur_time = GetNextEvent(scenarios, decisions, cur_time); // get the next event in the real scenario
        //printf("Next Time:%.1lf \n", cur_time);
        prev_decisions = decisions; // set the previous decisions
        //decisions.Show();
        prev_decisions.Remove(cur_time); // remove the decisions at the current time
        //printf("\n\n");
        //prev_decisions.Show();
        //getchar();
    }


    Prob<Node, Driver> real; 
    scenarios.GenerateReal(real, Parameters::GetTimeHorizon() + 1); // generate the real scenario
    Parameters::SetCurrentTime(0); // set the current time to 0
    Solver solver(real); // create the solver
    solver.SetDecisions(prev_decisions); // set the previous decisions
    //solver.Optimize();
    solver.Unfix(); // unfix the problem
    //solver.Show();
    //prev_decisions.Show();

    solver.Update(); // update the problem
    cost = solver.cost; // get the cost of the solution
    unassigneds = solver.nb_unassigneds / 2; // get the number of unassigned customers
    distance = solver.distances; // get the distance of the solution
    nb_routes = solver.GetRouteCount(); // get the number of routes
    time_taken = chrono.getTime(); // get the time taken to solve the problem

    //Show();
    //printf("FutureCustomerAssignments:%d\n", );
    //printf("KnownCustomerUnassignments:%d\n", solver.GetInitialCustomerUnassignmentCount());

    printf("Dynamic ");
    printf("Cost:%.2lf Dist:%.2lf Un:%d Routes:%d ", cost, distance, unassigneds, nb_routes);
    printf("Time:%.3lf Events:%d\n", time_taken, nb_events);

    final_solution.name = "Dynamic"; // set the name of the solution
    if (!_forbid_stochastic_drop_after_real) // if stochastic drop after real requests is allowed
        final_solution.name.append(" WS"); // append the name of the solution
    if (_allow_en_route_returns) // if en route returns are allowed
        final_solution.name.append(" PDR"); // append the name of the solution
    solver.GetSolutionCompact(final_solution);// get the solution in compact form
    //solver.Show();
    solver.GetReport(report);   // get the report of the solution
}

double DynSimulation::GetNextEvent(Scenarios &scenarios, Decisions &decs, double cur_time) {
    double next_request = scenarios.GetNextEvent(cur_time);
    if (!Parameters::AllowEnRouteDepotReturns()) return next_request;

    double next_delivery = Parameters::GetTimeHorizon() + 999;
    for (int i = 0; i < decs.GetCount(); i++) {
        Decision *d = decs.Get(i);
        if (d->decision_type != DECISION_TYPE_ROUTING) continue;
        if (!d->is_real) continue;

        if (d->node_type == NODE_TYPE_DROP && d->departure_time > cur_time)
            next_delivery = std::min(next_delivery, d->departure_time);
    }

    return std::min(next_request, next_delivery);
}
