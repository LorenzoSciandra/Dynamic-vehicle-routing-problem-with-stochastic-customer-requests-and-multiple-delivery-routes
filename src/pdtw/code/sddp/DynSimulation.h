#ifndef DYNSIMULATION_H
#define DYNSIMULATION_H

#include "Scenarios.h"
#include "Decisions.h"
#include "Parameters.h"
#include <vector>
#include "../SolCompact.h"
#include "Report.h"
#include "Test.h"

class DynSimulation {
public:

    double cost;
    double time_taken;
    int unassigneds;
    double distance;
    int nb_routes;
    int nb_events;

    bool _forbid_stochastic_drop_after_real;
    bool _allow_en_route_returns;
    SolCompact<Node, Driver> final_solution;
    Report report;

    DynSimulation() {}


    void Optimize(Scenarios &scenarios);

    void Show() {
        printf("Dynamic ");
        printf("Cost:%.2lf Dist:%.2lf Un:%d Routes:%d ", cost, distance, unassigneds, nb_routes);
        printf("Time:%.3lf Events:%d ", time_taken, nb_events);
        printf("AllowDepotReturns:%d\n", (int) _allow_en_route_returns);
    }

    void ShowReport() {
        report.Show();
    }

    result GetResult() {
        result r;
        r.id = 3;
        r.nb_customers = Parameters::GetRealRequestCount();
        r.distance = distance;
        r.unassigneds = unassigneds;
        r.time_taken = time_taken;
        r.nb_routes = nb_routes;
        r.nb_events = nb_events;
        r.allow_pdr = _allow_en_route_returns;
        r.alns = Parameters::GetAlnsIterations();
        r.driver_count = Parameters::GetDriverCount();
        r.time_horizon = Parameters::GetP();
        r.scenario_count = Parameters::GetScenarioCount();
        r.time_horizon_type = Parameters::GenerateIntelligentScenario() ? 1 : 0;
        r.name = "Reopt";
        r.idle_rate = report.GetIdleRate();
        return r;
    }

private:
    double GetNextEvent(Scenarios &scenarios, Decisions &decs, double cur_time);
};


#endif
