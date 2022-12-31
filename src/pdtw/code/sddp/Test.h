/*
 * Copyright Jean-Francois Cote 2021
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
*/
#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <string>

class result {
public:
    int id;
    int nb_customers;
    double distance;
    int unassigneds;
    double time_taken;
    int nb_routes;
    int nb_events;
    int unassigneds_oa; //in B&R, customers that we accepted but couldn't deliver
    int nb_branchings;
    int nb_evaluated_nodes;
    std::string name;
    bool allow_pdr;
    int alns;
    int driver_count;
    int time_horizon;
    int scenario_count;
    int time_horizon_type;
    double idle_rate;

    result() {
        id = -1;
        nb_customers = -1;
        distance = -1;
        unassigneds = -1;
        time_taken = -1;
        nb_routes = -1;
        nb_events = -1;
        unassigneds_oa = -1;
        nb_branchings = -1;
        nb_evaluated_nodes = -1;
        time_horizon_type = -1;
        allow_pdr = false;
        idle_rate = -1;
        alns = Parameters::GetAlnsIterations();
        driver_count = Parameters::GetDriverCount();
        time_horizon = Parameters::GetP();
        scenario_count = Parameters::GetScenarioCount();
    }

    result(int i, int c, double d, int u, double t, int r, int e) :
            id(i), nb_customers(c), distance(d), unassigneds(u), time_taken(t), nb_routes(r), nb_events(e),
            unassigneds_oa(-1), nb_branchings(-1) {}

    result(int i, int c, double d, int u, double t, int r, int e, int oa) :
            id(i), nb_customers(c), distance(d), unassigneds(u), time_taken(t), nb_routes(r), nb_events(e),
            unassigneds_oa(oa), nb_branchings(-1) {}

    void Show() {
        /*if(id == 1) printf("Static ");
        else if(id == 2) printf("Dynamic No Return ");
        else if(id == 3) printf("Dynamic Return ");
        else if(id == 4) printf("Consensus Stacy No Return No Wait ");
        else if(id == 5) printf("Consensus Stacy No Return Wait ");
        else if(id == 6) printf("Consensus Stacy Return Wait ");
        else if(id == 7) printf("Consensus By Driver ");
        else if(id == 8) printf("Consensus MinDiff ");
        else if(id == 9) printf("Branch&Regret ");*/

        printf("Alns:%4d Drivers:%2d TH:%3d SC:%2d Algo:%s ", alns, driver_count, time_horizon, scenario_count,
               name.c_str());
        printf("Dist:%.2lf Un:%d Routes:%d Time:%.2lf Events:%d ", distance, unassigneds, nb_routes, time_taken,
               nb_events);
        printf("Idle:%.3lf ", idle_rate);
        if (allow_pdr) printf("PDR:Yes ");
        else printf("PDR:No ");
        printf("ThType:%d ", time_horizon_type);
        if (id == 9 && nb_branchings != -1)
            printf("Branch:%d ", nb_branchings);
        if (id == 9 && nb_evaluated_nodes != -1)
            printf("Nodes:%d ", nb_evaluated_nodes);
        if (id == 9 && unassigneds_oa != -1)
            printf("UnOA:%d", unassigneds_oa);
        printf("\n");
    }

    void Print(FILE *f) {
        fprintf(f, "%d;%d;%d;%d;%s;", alns, driver_count, time_horizon, scenario_count, name.c_str());
        fprintf(f, "%.3lf;%d;%.3lf;%d;%d;", distance, unassigneds, time_taken, nb_routes, nb_events);
        double filled = (nb_customers - unassigneds) / ((double) nb_customers);
        double te = time_taken / ((double) nb_events);
        fprintf(f, "%.4lf;%.4lf;", filled, te);
        if (allow_pdr) fprintf(f, "Yes;"); else fprintf(f, "No;");
        fprintf(f, "%d;", time_horizon_type);
        fprintf(f, "%d;", nb_branchings);
        fprintf(f, "%d;", nb_evaluated_nodes);
        fprintf(f, "%.4lf;", idle_rate);
        //fprintf(f,"%d;", unassigneds_oa);
    }
};

class config {
public:
    int alns;
    int driver_count;
    int time_horizon;
    int scenario_count;
    std::vector <result> results;

    config(int a, int d, int t, int s) : alns(a), driver_count(d), time_horizon(t), scenario_count(s) {}

    void Add(result &r) { results.push_back(r); }

    void Add(result r) { results.push_back(r); }

    void Show() {
        printf("\nAlns:%d Drivers:%d ", alns, driver_count);
        printf("Horizon:%d Scenarios:%d \n", time_horizon, scenario_count);
        for (size_t i = 0; i < results.size(); i++)
            results[i].Show();
    }

    void Print(FILE *f) {
        fprintf(f, "%d;%d;%d;%d;", alns, driver_count, time_horizon, scenario_count);
        for (size_t i = 0; i < results.size(); i++)
            results[i].Print(f);
        fprintf(f, ";");
    }
};

#endif
