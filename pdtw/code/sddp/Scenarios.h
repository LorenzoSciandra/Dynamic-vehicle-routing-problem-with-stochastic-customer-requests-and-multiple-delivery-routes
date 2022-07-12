#ifndef _SCENARIOS_H
#define _SCENARIOS_H


#include <vector>
#include "../ProblemDefinition.h"
#include "Scenario.h"
#include "NodeSddp.h"
#include "DriverSddp.h"


class Scenarios {
private:
    Scenario real; //real scenario
    std::vector <Scenario> scenarios; // list of possible future scenarios
    Node depot;
    Driver driver;
    double max_distance_from_depot;

    int time_window_type;
    int service_time;
    int depot_start_time;
    int depot_end_time;
    int service_time_depot;
    int service_time_customer;


public:
    Scenarios() {
        depot_start_time = 0;
        depot_end_time = 9999999;
        service_time = 0;
    }

    void Load(char *filename);

    void LoadStacyVoccia(std::string filename);

    void LoadStacyVocciaSubFile(std::string filename);

    void LoadUlmer(std::string filename);

    void GenerateReal(Prob<Node, Driver> &prob, double time);

    void Generate(std::vector <Prob<Node, Driver>> &probs, double time);

    void GenerateUlmerScenarios(Scenarios &sc, int real_sc_no);

    void UpdateUlmerDepot(int depot_id);

    Node *GetDepot() { return &depot; }

    int GetScenarioCount() { return (int) scenarios.size(); }

    Scenario *GetScenario(int i) { return &scenarios[i]; }

    int GetRealScenarioCustomerCount() { return real.GetNodeCount(); }

    Node *GetRealScenarioCustomer(int i) { return real.GetNode(i); }

    double GetNextEvent(double time) {
        return real.GetNextEvent(time);
    }

    bool IsStillFeasible(Node *n);

    bool CanWait(Node *n);

    void Show();

    std::vector <std::string> info;
    std::string path_input;
    std::string path_output;
    std::string problem_name;
    std::string problem_name_tw;
    std::string actual_scenario_id;
    bool is_stacy = false;
    bool is_ulmer = false;
    int nb_real_requests_instance; //number of requests in the file
};


#endif
