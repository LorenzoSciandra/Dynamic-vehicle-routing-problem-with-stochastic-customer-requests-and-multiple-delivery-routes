
/*
 * Copyright Jean-Francois Cote 2019
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 *
 * If you have improvements, please contact me!
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "Scenarios.h"
#include "Solver.h"
#include "BranchRegretSimulation.h"
#include "DynSimulation.h"
#include "DynSimulationUlmer.h"
#include "StaticSimulation.h"
#include "ConsensusSimulation.h"
#include <vector>

class result {
public:
    int id;
    double distance;
    int known_unassignments;
    int future_assignments;
    int future_customers;
    double time_taken;
    int nb_routes;
    int nb_events;
    int nb_customers;

    result(int i, double d, int fc, int fa, int ku, double t, int r, int e, int c) : id(i), distance(d),
                                                                                     future_customers(fc),
                                                                                     future_assignments(fa),
                                                                                     known_unassignments(ku),
                                                                                     time_taken(t), nb_routes(r),
                                                                                     nb_events(e), nb_customers(c) {}


    void Show() {
        if (id == 1) printf("Static ");
        else if (id == 2) printf("Dynamic No Return ");
        else if (id == 3) printf("Dynamic Return ");
        else if (id == 4) printf("Consensus Stacy No Return No Wait ");
        else if (id == 5) printf("Consensus Stacy No Return Wait ");
        else if (id == 6) printf("Consensus Stacy Return Wait ");
        else if (id == 7) printf("Consensus By Driver ");
        else if (id == 8) printf("Consensus MinDiff ");
        else if (id == 9) printf("Branch&Regret ");

        printf("Cust:%d Dist:%.2lf FC:%d KU:%d FA:%d Routes:%d Time:%.2lf Events:%d ", nb_customers, distance,
               future_customers, known_unassignments, future_assignments, nb_routes, time_taken, nb_events);
        printf("\n");
    }

    void Print(FILE *f) {
        fprintf(f, "%.2lf;%d;%d;%.3lf;%d;%d;", distance, known_unassignments, future_assignments, time_taken, nb_routes,
                nb_events);
    }
};

class config {
public:
    int alns;
    int driver_count;
    int time_horizon;
    int scenario_count;
    int depot;
    std::vector <result> results;

    config(int a, int d, int t, int s, int dep) : alns(a), driver_count(d), time_horizon(t), scenario_count(s),
                                                  depot(dep) {}

    void Show() {
        printf("Depot:%d Alns:%d Drivers:%d ", depot, alns, driver_count);
        printf("Horizon:%d Scenarios:%d \n", time_horizon, scenario_count);
        //for(size_t i=0;i<results.size();i++)
        //	results[i].Show();

        int max_id = -1;
        for (size_t i = 0; i < results.size(); i++)
            max_id = std::max(max_id, results[i].id);

        std::vector<double> dist(max_id + 1, 0);
        std::vector<double> fc(max_id + 1, 0);
        std::vector<double> ku(max_id + 1, 0);
        std::vector<double> fa(max_id + 1, 0);
        std::vector<double> customers(max_id + 1, 0);
        std::vector<double> routes(max_id + 1, 0);
        std::vector<double> time(max_id + 1, 0);
        std::vector<double> events(max_id + 1, 0);
        std::vector<int> nb(max_id + 1, 0);
        std::vector<double> filled(max_id + 1, 0);

        for (size_t j = 0; j < results.size(); j++) {
            dist[results[j].id] += results[j].distance;
            customers[results[j].id] += results[j].nb_customers;
            fc[results[j].id] += results[j].future_customers;
            ku[results[j].id] += results[j].known_unassignments;
            fa[results[j].id] += results[j].future_assignments;
            if (results[j].future_customers >= 1)
                filled[results[j].id] += results[j].future_assignments / (double) results[j].future_customers;
            routes[results[j].id] += results[j].nb_routes;
            time[results[j].id] += results[j].time_taken;
            events[results[j].id] += results[j].nb_events;
            nb[results[j].id]++;
        }

        for (int i = 0; i <= max_id; i++)
            if (nb[i] >= 1) {
                dist[i] /= nb[i];
                filled[i] /= nb[i];
                ku[i] /= nb[i];
                fc[i] /= nb[i];
                fa[i] /= nb[i];
                routes[i] /= nb[i];
                time[i] /= nb[i];
                events[i] /= nb[i];
                customers[i] /= nb[i];
                printf("ID:%d Average Dist:%.2lf Cust:%.3lf FC:%.2lf KU:%.6lf FA:%.2lf Routes:%.2lf Time:%.2lf Events:%.2lf Filled:%.3lf\n",
                       i, dist[i], customers[i], fc[i], ku[i], fa[i], routes[i], time[i], events[i], filled[i]);
            }
    }

    void Print(FILE *f) {
        fprintf(f, "%d;%d;%d;%d;%d;", depot, alns, driver_count, time_horizon, scenario_count);
        for (size_t i = 0; i < results.size(); i++)
            results[i].Print(f);
        fprintf(f, ";");
    }

    void PrintAverages(FILE *f) {
        fprintf(f, "%d;%d;%d;%d;%d;", depot, alns, driver_count, time_horizon, scenario_count);
        int max_id = -1;
        for (size_t i = 0; i < results.size(); i++)
            max_id = std::max(max_id, results[i].id);
        std::vector<double> dist(max_id + 1, 0);
        std::vector<double> customers(max_id + 1, 0);
        std::vector<double> fc(max_id + 1, 0);
        std::vector<double> ku(max_id + 1, 0);
        std::vector<double> fa(max_id + 1, 0);
        std::vector<double> routes(max_id + 1, 0);
        std::vector<double> time(max_id + 1, 0);
        std::vector<double> events(max_id + 1, 0);
        std::vector<int> nb(max_id + 1, 0);
        std::vector<double> filled(max_id + 1, 0);

        for (size_t j = 0; j < results.size(); j++) {
            dist[results[j].id] += results[j].distance;
            customers[results[j].id] += results[j].nb_customers;
            fc[results[j].id] += results[j].future_customers;
            ku[results[j].id] += results[j].known_unassignments;
            fa[results[j].id] += results[j].future_assignments;
            if (results[j].future_customers >= 1)
                filled[results[j].id] += results[j].future_assignments / (double) results[j].future_customers;
            routes[results[j].id] += results[j].nb_routes;
            time[results[j].id] += results[j].time_taken;
            events[results[j].id] += results[j].nb_events;
            nb[results[j].id]++;
        }

        for (int i = 0; i <= max_id; i++)
            if (nb[i] >= 1) {
                dist[i] /= nb[i];
                filled[i] /= nb[i];
                fc[i] /= nb[i];
                ku[i] /= nb[i];
                fa[i] /= nb[i];
                routes[i] /= nb[i];
                time[i] /= nb[i];
                events[i] /= nb[i];
                customers[i] /= nb[i];

                fprintf(f, "%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;%.3lf;", dist[i], ku[i], fa[i], routes[i], time[i],
                        events[i], filled[i]);
            }
        fprintf(f, ";");
    }
};


void Execute(Scenarios &scenarios, config &conf);

int main(int arg, char **argv) {
    time_t tt = time(0);
    tt = 15646022;
    srand(tt);
    printf("Seed:%ld\n", tt);
    //srand(1304445903);

    Parameters::SetUlmerDepotLocation(2);

    Scenarios scenarios_all;
    if (arg > 3)
        scenarios_all.LoadUlmer(argv[1]);
    else {
        printf("Provide an instance as parameter\n");
        printf("Provide a depot id\n");
        printf("Provide an output file\n");
        exit(1);
    }

    int depot = 1;
    sscanf(argv[2], "%d", &depot);


    std::vector <config> configs;
    configs.push_back(config(100, 1, 0, 1, depot));
    //configs.push_back(config(10,1,0,1,2));
    //configs.push_back(config(10,1,0,1,3));


    for (size_t i = 0; i < configs.size(); i++)
        for (int j = 0; j < scenarios_all.GetScenarioCount(); j++)
            //for(int j=0;j<100;j++)
            //for(int j=39;j<=40;j++)
        {
            Scenarios sc;
            scenarios_all.GenerateUlmerScenarios(sc, j);
            printf("Scenario:%d\n", j);
            Execute(sc, configs[i]);
        }


    double avg_customers = 0;
    for (int j = 0; j < scenarios_all.GetScenarioCount(); j++)
        avg_customers += scenarios_all.GetScenario(j)->GetNodeCount() / 2;
    avg_customers /= scenarios_all.GetScenarioCount();

    printf("File:%s AvgCustomers:%.2lf\n", argv[1], avg_customers);
    for (size_t i = 0; i < configs.size(); i++)
        configs[i].Show();


    if (arg >= 3) {
        FILE *f = fopen(argv[3], "w");
        if (f != NULL) {
            fprintf(f, "%s;%.2lf;", argv[1], avg_customers);
            for (size_t i = 0; i < configs.size(); i++)
                configs[i].PrintAverages(f);
            fprintf(f, "\n");
            fclose(f);
        }
    }

    return 0;
}


void Execute(Scenarios &scenarios, config &conf) {
    Parameters::SetAlnsIterations(conf.alns);
    Parameters::SetDriverCount(conf.driver_count);
    Parameters::SetScenarioCount(conf.scenario_count);
    Parameters::SetP(conf.time_horizon);
    Parameters::SetGenerateIntelligentScenario(false);
    Parameters::SetOrderAcceptancy(false);                    //for branch-and-regret
    Parameters::SetUlmerDepotLocation(conf.depot);
    scenarios.UpdateUlmerDepot(conf.depot);

    printf("\n\nExecute AlnsIterations:%d Drivers:%d ", Parameters::GetAlnsIterations(), Parameters::GetDriverCount());
    printf("P:%d Scenarios:%d Customers:%d Depot:%d\n", Parameters::GetP(), Parameters::GetScenarioCount(),
           scenarios.GetRealScenarioCustomerCount() / 2, conf.depot);

    int custs = scenarios.GetRealScenarioCustomerCount() / 2;
    /*{
    Parameters::SetStaticAlnsIterations(150);
    StaticSimulation sim;
    sim.Optimize(scenarios);
    int fc = sim.future_customers;
    int fa = sim.future_assignments;
    int ku = sim.known_unassignments;
    conf.results.push_back( result(1,sim.distance,fc,fa,ku,sim.time_taken,sim.nb_routes, sim.nb_events,custs) );
}*/

    {
        DynSimulationUlmer sim;
        Parameters::SetAllowEnRouteDepotReturns(false);
        sim.Optimize(scenarios);
        int fc = sim.future_customers;
        int fa = sim.future_assignments;
        int ku = sim.known_unassignments;
        conf.results.push_back(
                result(2, sim.distance, fc, fa, ku, sim.time_taken, sim.nb_routes, sim.nb_events, custs));
    }

    {
        DynSimulationUlmer sim;
        Parameters::SetAllowEnRouteDepotReturns(true);
        sim.Optimize(scenarios);
        int fc = sim.future_customers;
        int fa = sim.future_assignments;
        int ku = sim.known_unassignments;
        conf.results.push_back(
                result(3, sim.distance, fc, fa, ku, sim.time_taken, sim.nb_routes, sim.nb_events, custs));
    }

/*
	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	Parameters::SetAllowEnRouteDepotReturns(true);
	Parameters::SetForbidStochasticDropAfterReal(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(6,sim.distance,sim.future_assignments, sim.known_unassignments, sim.time_taken,sim.nb_routes, sim.nb_events,custs );
	}


	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_MINIMAL_CHANGE );
	Parameters::SetAllowEnRouteDepotReturns(true);
	Parameters::SetForbidStochasticDropAfterReal(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(8,sim.distance,sim.future_assignments, sim.known_unassignments, sim.time_taken,sim.nb_routes, sim.nb_events,custs) );
}*/


    /*{
    BranchRegretSimulation sim;
    Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
    Parameters::SetAllowEnRouteDepotReturns(true);
    Parameters::SetForbidStochasticDropAfterReal(false);
    Parameters::SetOrderAcceptancy(false);
    Parameters::SetEvaluateWaitingStrategy(true);
    Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
    Parameters::SetGenerateIntelligentScenario(true);
    Parameters::SetP(0);
    sim.Optimize(scenarios);
    conf.results.push_back( result(9,sim.distance,sim.future_assignments, sim.known_unassignments, sim.time_taken,sim.nb_routes, sim.nb_events,custs) );
    conf.results.back().nb_branchings = sim.nb_branchings;
}*/

}
