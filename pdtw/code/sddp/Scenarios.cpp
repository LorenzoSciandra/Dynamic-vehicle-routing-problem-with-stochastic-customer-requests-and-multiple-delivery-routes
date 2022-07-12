#include <iostream>
#include <fstream>
#include <algorithm>
#include "Scenarios.h"
#include "Parameters.h"
#include "../constants.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cmath>

#define _CRT_SECURE_NO_WARNINGS
using namespace std;
namespace patch {
    template<typename T>
    std::string to_string(const T &n) {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}

void Scenarios::Load(char *filename) {
    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        printf("Error at the opening of the file:%s\n", filename);
        exit(1);
    }

    int id, origin_id, nb_items, cap, duration, vehicle_capacity, start_time, end_time, number_areas, number_scenarios, type_scenario, number_clients, number_drivers, pick_node_id, del_node_id, k;
    int dmd, sum_dmd = 0;
    float release_time, x_dep, y_dep, x, y, driversSpeed;
    char line1[30];
    char line2[30];
    char line3[30];
    char line4[30];
    char line5[30];
    char line6[30];
    char line7[30];

    int rrr = 0;
    char *rrc = NULL;

    rrr = fscanf(f, "%s %s %s %d", line1, line2, line3, &number_scenarios);
    rrr = fscanf(f, "%s %s %s %d %s %s %s %f", line1, line2, line3, &number_drivers, line4, line5, line6,
                 &driversSpeed);
    rrr = fscanf(f, "%s %s %s %d", line1, line2, line3, &vehicle_capacity);
    rrr = fscanf(f, "%s %s %s %s", line1, line2, line3, line4);
    rrr = fscanf(f, "%s %s %f %s %f %s %s %d %s %d %s\n", line1, line2, &x_dep, line3, &y_dep, line4, line5,
                 &start_time, line6, &end_time, line7);
    Node dep;
    dep.x = x_dep;
    dep.y = y_dep;
    dep.tw_start = 480;
    dep.tw_end = 1080;
    rrr = fscanf(f, "%s %s %s %d", line1, line2, line3, &number_areas);
    rrr = fscanf(f, "%s %s %s %s", line1, line2, line3, line4);
    depot = dep;
    for (int i = 0; i < number_scenarios; i++) {
        Scenario s;
        s.start_time = start_time * 60;
        s.end_time = end_time * 60;
        rrr = fscanf(f, "%s %s %s %s %d %s %s %s %d", line1, line2, line3, line4, &type_scenario, line5, line6, line7,
                     &number_clients);
        rrr = fscanf(f, "%s %s %s %s %s", line1, line2, line3, line4, line5);
        for (int j = 0; j < number_clients; j++) {
            Node n;
            rrr = fscanf(f, "%d %f %f %d %f", &id, &x, &y, &dmd, &release_time);
            n.id = id - 1;
            n.origin_id = id;
            n.distID = id;
            if (type_scenario == 1)
                n.is_real = true;
            else n.is_real = false;
            n.type = NODE_TYPE_CUSTOMER;
            n.serv_time = service_time;
            n.x = x;
            n.y = y;
            n.demand = dmd;
            n.release_time = release_time;
            s.AddNode(n);
        }
        if (type_scenario == 1)
            real = s;
        else
            scenarios.push_back(s);

    }
    fclose(f);
}


void Scenarios::LoadStacyVoccia(std::string filename) {
    Parameters::SetTimeHorizonStartTime(0);
    Parameters::SetInstanceType(INSTANCE_TYPE_STACY);
    is_stacy = true;
    is_ulmer = false;
    service_time_depot = 0;
    service_time_customer = 0;
    info.clear();

    std::ifstream myfile(filename.c_str());
    int lead_time, capacity, alns_iteration_count, real_weight;
    std::string ins_type;

    std::string word;
    std::string scenario_id;
    std::string tw_type;
    int time_horizon = 540;
    time_window_type = -1;

    bool prob_name = false;
    if (myfile.is_open()) {
        while (!myfile.eof()) {
            myfile >> word;
            //if(word == "NUM_SCENARIOS") { int sc;myfile >> sc; Parameters::SetScenarioCount(sc);}
            if (word == "ACTUAL_SCENARIO_ID") {
                myfile >> scenario_id;
                actual_scenario_id = scenario_id;
                while (scenario_id.length() != 3)
                    scenario_id = "0" + scenario_id;
                info.push_back(scenario_id);
            }
            if (word == "TW_TYPE" && prob_name) {
                myfile >> tw_type;
                problem_name_tw = problem_name + "_" + tw_type;

                //info.push_back(word);
                if (tw_type == "d1")
                    time_window_type = 0;
                else if (tw_type == "d2")
                    time_window_type = 1;
                else if (tw_type == "f")
                    time_window_type = 2;
                else if (tw_type == "d0")
                    time_window_type = 3;

            }
            if (word == "SERVICE_TIME") { myfile >> service_time; }
            if (word == "NB_DRIVERS") {
                int nb_drivers;
                myfile >> nb_drivers;
                Parameters::SetDriverCount(nb_drivers);
            }
            if (word == "INPUT_PATH") { myfile >> path_input; }
            if (word == "OUTPUT_PATH") { myfile >> path_output; }
            if (word == "REAL_REQUEST_WEIGHT") {
                myfile >> real_weight;
                Parameters::SetRealRequestWeight(real_weight);
            }
            if (word == "INSTANCE_TYPE") {
                myfile >> ins_type;/* cout << "Ins " << ins_type << endl;*/ if (ins_type == "Stacy")
                    Parameters::SetStacyInstance(true);
                if (ins_type == "Own_Instance")Parameters::SetStacyInstance(false);
            }
            //if(word == "CAPACITY") { myfile >> capacity; Parameters::SetCapacity(capacity); }
            if (word == "ALNS_ITERATION_COUNT") {
                myfile >> alns_iteration_count;
                Parameters::SetAlnsIterations(alns_iteration_count);
            }
            if (word == "PROBLEM_NAME") {
                myfile >> problem_name;
                problem_name_tw = problem_name;
                //cout << problem_name << endl;
                info.push_back(problem_name);
                if (problem_name == "ACTUAL_SCENARIO_ID" || problem_name == "NB_DRIVERS") {
                    cout << "Problem name not found" << endl;
                    getchar();
                }
                if (problem_name.find("TWdf") != string::npos)
                    prob_name = true;
            }

            if (word == "HALF_TIME_HORIZON") {
                myfile >> word;
                if (word == "Yes")
                    Parameters::SetP(540 / 2);
            }

        }

        depot_start_time = 0;
        depot_end_time = time_horizon;

        Parameters::SetTimeHorizonStartTime(depot_start_time);
        Parameters::SetTimeHorizon(depot_end_time);
    } else {
        cout << "File not found : " << filename << endl;
        exit(1);
    }
    cout << "Instace Type:Stacy Problem:" << problem_name_tw << " Actual:" << scenario_id << " TwType:" << tw_type
         << endl;

    if (prob_name && time_window_type == -1) {
        cout << "Time Window: TWdf      Type: Not Found" << endl;
        cout << "Check in the configuration file if a type has been entered" << endl;
        getchar();
    }
    {
        std::string type;
        if (problem_name.find("hom") != string::npos) type = "Homogeneous"; else type = "Heterogeneous";

        max_distance_from_depot = 0;
        for (int i = 1; i <= 30; i++) {
            std::string sce = "0";
            if (i < 10) sce += "0";
            sce += std::to_string(i);
            LoadStacyVocciaSubFile(
                    (string) "../instances/sddp/stacy/" + type + "/" + problem_name + "_actual/" + problem_name +
                    "_actual_" + sce + ".txt");

            for (int j = 0; j < real.GetNodeCount(); j++) {
                if (real.GetNode(j)->type != NODE_TYPE_DROP) continue;
                Node *n1 = real.GetNode(j);
                double dd = std::abs(depot.x - n1->x) + std::abs(depot.y - n1->y);
                max_distance_from_depot = std::max(dd, max_distance_from_depot);
            }
        }

        LoadStacyVocciaSubFile(
                (string) "../instances/sddp/stacy/" + type + "/" + problem_name + "_actual/" + problem_name +
                "_actual_" + scenario_id + ".txt");

        for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
            scenario_id = patch::to_string(1 + i);
            while (scenario_id.length() != 3)
                scenario_id = "0" + scenario_id;
            // cout<<"../instances/Data_Sets/" + type + "/" + problem_name + "_sampled/" + problem_name + "_sampled_" + scenario_id + ".txt"<<endl;
            LoadStacyVocciaSubFile(
                    (string) "../instances/sddp/stacy/" + type + "/" + problem_name + "_sampled/" + problem_name +
                    "_sampled_" + scenario_id + ".txt");

            Scenario &sc = scenarios.back();
            for (int j = 0; j < sc.GetNodeCount(); j++) {
                if (sc.GetNode(j)->type != NODE_TYPE_DROP) continue;
                Node *n1 = sc.GetNode(j);
                double dd = std::abs(depot.x - n1->x) + std::abs(depot.y - n1->y);
                max_distance_from_depot = std::max(dd, max_distance_from_depot);
            }
        }
    }

    printf("MaxDistanceToDepot:%.1lf\n", max_distance_from_depot);
}

void Scenarios::LoadStacyVocciaSubFile(string filename) {
    is_stacy = true;
    std::ifstream myfile(filename.c_str());
    //cout << filename << endl;
    int id, origin_id, nb_items, cap, duration, serv_time, vehicle_capacity, start_time, end_time, number_areas, number_scenarios, type_scenario, number_clients, number_drivers, pick_node_id, del_node_id, k;
    int dmd, sum_dmd = 0;
    double time_s;
    float release_time, x_dep, y_dep, x, y, driversSpeed;
    int number;
    char word[30];

    bool actual_str = (filename.find("actual") == string::npos) ? false : true;
    bool TWh_str = (filename.find("TWh") == string::npos) ? false : true;
    bool TWr_str = (filename.find("TWr") == string::npos) ? false : true;
    bool TW3h_str = (filename.find("TW3h") == string::npos) ? false : true;
    bool TW3r_str = (filename.find("TW3r") == string::npos) ? false : true;

    if (actual_str)
        nb_real_requests_instance = 0;

    if (myfile.is_open()) {
        Scenario s;
        for (int i = 0; i < 4; i++)
            myfile >> word;
        if (TWh_str || TWr_str || TW3h_str || TW3r_str)
            for (int i = 0; i < 2; i++)
                myfile >> word;
        myfile >> number;
        myfile >> x_dep;
        myfile >> y_dep;
        myfile >> number;
        if (TWh_str || TWr_str || TW3h_str || TW3r_str) {
            myfile >> time_s;
            depot_start_time = s.start_time = time_s;
            Parameters::SetTimeHorizonStartTime(depot_start_time);
            myfile >> time_s;
            depot_end_time = s.end_time = time_s;
            Parameters::SetTimeHorizon(time_s);
        } else {
            int tt = 540;
            depot_start_time = s.start_time = 0;
            depot_end_time = s.end_time = tt;
            Parameters::SetTimeHorizon(tt);
        }
        Node dep;
        dep.x = x_dep;
        dep.y = y_dep;
        dep.pick_node_id = -1;
        dep.del_node_id = -1;
        depot = dep;
        while (!myfile.eof()) {
            int startingValue = -1;
            myfile >> startingValue;
            if (startingValue != -1) {
                if (actual_str) nb_real_requests_instance++;
                Node n1;
                n1.id = s.GetNodeCount();
                n1.distID = 0;
                n1.origin_id = startingValue;
                n1.is_real = actual_str;
                n1.type = NODE_TYPE_PICKUP;
                n1.x = depot.x;
                n1.y = depot.y;

                Node n;
                n.id = s.GetNodeCount() + 1;
                n.distID = startingValue;
                n.origin_id = n.distID;
                n1.is_real = actual_str;
                n.type = NODE_TYPE_DROP;
                myfile >> n.x;
                myfile >> n.y;
                myfile >> n.release_time;
                if (n.release_time > 540) {
                    cout << "Release Time > 540 in " << info[0] << " " << info[1] << endl;
                    //exit();
                }

                n.pick_node_id = n1.id;

                if (TWh_str || TWr_str || TW3h_str || TW3r_str) {
                    myfile >> n.tw_start;
                    myfile >> n.tw_end;
                    n1.tw_start = n.release_time;
                    n1.tw_end = n.tw_end;
                } else {
                    if (time_window_type == 0) {
                        n.tw_start = n.release_time;
                        n.tw_end = n.tw_start + 60;
                        n1.tw_start = n.release_time;
                        n1.tw_end = n.tw_end;
                    }
                    if (time_window_type == 1) {
                        n.tw_start = n.release_time;
                        n.tw_end = n.tw_start + 120;
                        n1.tw_start = n.release_time;
                        n1.tw_end = n.tw_end;
                    }
                    if (time_window_type == 2) {
                        n.tw_start = n.release_time + 60;
                        n.tw_end = n.tw_start + 60;
                        n1.tw_start = n.release_time;
                        n1.tw_end = n.tw_end;
                    }
                    if (time_window_type == 3) {
                        n.tw_start = n.release_time;
                        n.tw_end = 540;
                        n1.tw_start = n.release_time;
                        n1.tw_end = 540;
                    }
                    if (time_window_type == -1) {//set to 3 by default
                        cout << "Time Window: TWdf      Type: Not Found" << endl;
                        cout << "Check in the configuration file if a type has been entered" << endl;
                        //exit();
                    }
                }
                if (time_window_type == 1 && n.release_time > 420 && n.release_time <= 480) {
                    n.tw_end = 540;
                    n1.tw_end = 540;
                }
                /*if(time_window_type == 1 && n.release_time>480)
                {

                    n.release_time = 480;
                    n1.release_time = 480;

                    n.tw_start = 480;
                    n.tw_end = 540;
                    n1.tw_start = 480;
                    n1.tw_end = 540;
                }*/
                if (time_window_type == 2 && n.release_time > 420 && n.release_time <= 480) {
                    n.tw_start = 480;
                    n.tw_end = 540;
                    //n1.tw_start = n.release_time;
                    n1.tw_end = 540;
                    if (n.release_time > 480) {
                        n.release_time = 480;
                        n1.release_time = 480;
                        n1.tw_start = 480;
                    }
                }
                if (n.release_time > n.tw_start) {
                    printf("Release_time higher than TwStart request:%d Rel:%d TwStart:%d\n", n.origin_id,
                           (int) n.release_time, (int) n.tw_start);
                    n.release_time = n.tw_start;
                    n1.release_time = n.tw_start;
                    exit(1);
                }

                n1.del_node_id = n.id;
                n1.release_time = n.release_time;

                if (n.release_time <= Parameters::GetTimeHorizon()) {
                    s.AddNode(n1);
                    s.AddNode(n);
                }
                if (n.release_time > Parameters::GetTimeHorizon()) {
                    printf("Infeasible request:%d file:%s\n", startingValue, filename.c_str());
                }
            }
        }

        if (actual_str) {
            real = s;
            Parameters::SetRealRequestCount(nb_real_requests_instance);
        } else
            scenarios.push_back(s);


        myfile.close();
    } else {
        cout << "File not found:" << filename << endl;
        exit(1);
    }
}

void Scenarios::LoadUlmer(std::string filename) {
    Parameters::SetTimeHorizonStartTime(-1);
    is_ulmer = true;
    is_stacy = false;
    Parameters::SetInstanceType(INSTANCE_TYPE_ULMER);
    std::ifstream myfile(filename.c_str());

    //Parameters::SetTimeHorizonStartTime(0);
    Parameters::SetTimeHorizon(480);

    depot_start_time = 0;
    depot_end_time = 480;
    service_time_depot = 5;
    service_time_customer = 2;
    depot.distID = 0;
    depot.pick_node_id = -1;
    depot.del_node_id = -1;
    depot.is_real = true;
    if (Parameters::GetUlmerDepotLocation() == 1) {
        depot.x = 5000;
        depot.y = 5000;
    } else if (Parameters::GetUlmerDepotLocation() == 2) {
        depot.x = 0;
        depot.y = 10000;
    } else if (Parameters::GetUlmerDepotLocation() == 3) {
        depot.x = 0;
        depot.y = 0;
    } else {
        cout << "Depot not defined" << endl;
        getchar();
    }

    int scenario_num, cust_num, time_req, x_coo, y_coo, sc = 0;


    if (myfile.is_open()) {

        myfile >> scenario_num;

        while (!myfile.eof()) {

            Scenario s;

            while (!myfile.eof() && sc == scenario_num) {
                myfile >> cust_num;
                myfile >> time_req;
                myfile >> x_coo;
                myfile >> y_coo;

                Node n_p;
                n_p.id = s.GetNodeCount();
                n_p.distID = 1;
                n_p.origin_id = cust_num + 1;
                n_p.is_real = false;
                n_p.type = NODE_TYPE_PICKUP;
                n_p.x = depot.x;
                n_p.y = depot.y;
                n_p.release_time = time_req;
                n_p.tw_start = time_req;
                n_p.tw_end = Parameters::GetTimeHorizon();

                Node n_d;
                n_d.id = s.GetNodeCount() + 1;
                n_d.distID = cust_num + 2;
                n_d.origin_id = n_d.distID;
                n_d.is_real = false;
                n_d.type = NODE_TYPE_DROP;
                n_d.x = x_coo;
                n_d.y = y_coo;
                n_d.release_time = time_req;
                n_d.tw_start = time_req;
                n_d.tw_end = Parameters::GetTimeHorizon();

                n_d.pick_node_id = n_p.id;
                n_p.del_node_id = n_d.id;

                s.AddNode(n_p);
                s.AddNode(n_d);

                if (!myfile.eof())
                    myfile >> scenario_num;
            }
            if (s.GetNodeCount() != 0)
                scenarios.push_back(s);
            sc++;
        }
        myfile.close();
    } else {
        cout << "File not found:" << filename << endl;
        exit(1);
    }
}

void Scenarios::UpdateUlmerDepot(int depot_id) {
    if (depot_id == 1) {
        depot.x = 5000;
        depot.y = 5000;
    } else if (depot_id == 2) {
        depot.x = 0;
        depot.y = 10000;
    } else if (depot_id == 3) {
        depot.x = 0;
        depot.y = 0;
    }

    for (size_t k = 0; k < scenarios.size(); k++)
        for (int i = 0; i < scenarios[k].GetNodeCount(); i++) {
            Node *n = scenarios[k].GetNode(i);
            if (n->type == NODE_TYPE_PICKUP) {
                n->x = depot.x;
                n->y = depot.y;
            }
        }
    for (int i = 0; i < real.GetNodeCount(); i++) {
        Node *n = real.GetNode(i);
        if (n->type == NODE_TYPE_PICKUP) {
            n->x = depot.x;
            n->y = depot.y;
        }
    }
}

void Scenarios::Generate(std::vector <Prob<Node, Driver>> &probs, double time) {
    probs.clear();
    probs = std::vector<Prob<Node, Driver> >(Parameters::GetScenarioCount());

    int inf_request_count = 0;
    /*
    //As of 02-2021, it is done in the Load over all actual scenarios and fictive scenarios
    max_distance_from_depot = 0;
    for(int i=0;i<real.GetNodeCount();i++)
    {
        if(real.GetNode(i)->type != NODE_TYPE_DROP) continue;
        Node * n1 = real.GetNode(i);
        double dd = std::abs(depot.x - n1->x) + std::abs(depot.y - n1->y);
        max_distance_from_depot = std::max( dd, Scenarios::max_distance_from_depot);
    }

    for(int k=0;k<scenarios.size();k++)
        for(int j = 0; j < scenarios[k].GetNodeCount(); j++)
        {
            if(scenarios[k].GetNode(j)->type != NODE_TYPE_DROP) continue;
            Node * n1 = scenarios[k].GetNode(j);
            double dd = std::abs(depot.x - n1->x) + std::abs(depot.y - n1->y);
            Scenarios::max_distance_from_depot = std::max( dd, Scenarios::max_distance_from_depot);
        }*/

    for (int k = 0; k < Parameters::GetScenarioCount(); k++)
        probs[k].Reserve();

    for (int k = 0; k < Parameters::GetScenarioCount(); k++) {
        Prob<Node, Driver> *p = &probs[k];
        for (int j = 0; j < Parameters::GetDriverCount(); j++) {
            Node dep1(depot);
            dep1.x = depot.x;
            dep1.y = depot.y;
            dep1.id = p->GetNodeCount();
            dep1.distID = 0;
            dep1.type = NODE_TYPE_START_DEPOT;
            dep1.release_time = 0;
            dep1.is_real = true;
            dep1.arrival_time = 0;
            dep1.serv_time = 0;
            dep1.tw_start = depot_start_time;
            dep1.tw_end = depot_end_time;
            Node dep2(depot);
            dep2.x = depot.x;
            dep2.y = depot.y;
            dep2.id = dep1.id + 1;
            dep2.distID = 0;
            dep2.type = NODE_TYPE_END_DEPOT;
            dep2.is_real = true;
            dep2.tw_start = depot_start_time;
            dep2.tw_end = depot_end_time;

            Driver d(driver);
            d.capacity = 999999999;// cap;
            d.StartNodeID = dep1.id;
            d.last_done_node_id = dep1.id;
            d.EndNodeID = dep2.id;
            d.id = j;

            p->AddNode(dep1);
            p->AddNode(dep2);
            p->AddDriver(d);
        }

        int dist_id = 2;
        for (int j = 0; j < real.GetNodeCount(); j++)
            if (real.GetNode(j)->release_time <= time + 0.001 && real.GetNode(j)->type == NODE_TYPE_PICKUP) {
                Node n1 = *real.GetNode(j);
                Node n2 = *real.GetNode(n1.del_node_id);
                n1.id = p->GetNodeCount();
                n2.id = n1.id + 1;
                n1.distID = 1;
                n2.distID = dist_id++;
                n1.del_node_id = n2.id;
                n2.pick_node_id = n1.id;
                p->AddNode(n1);
                p->AddNode(n2);
            }
        for (int j = 0; j < scenarios[k].GetNodeCount(); j++)
            if (scenarios[k].GetNode(j)->type == NODE_TYPE_PICKUP &&
                scenarios[k].GetNode(j)->release_time > time + 0.1 &&
                (scenarios[k].GetNode(j)->release_time <= time + Parameters::GetP() ||
                 Parameters::GenerateIntelligentScenario())) {

                Node n1 = *scenarios[k].GetNode(j);
                Node n2 = *scenarios[k].GetNode(n1.del_node_id);

                //Check if the time window of the stochastic request intersect with one of the real customer
                if (Parameters::GenerateIntelligentScenario()) {
                    bool has_one = false;
                    for (int i = 0; i < real.GetNodeCount(); i++)
                        if (real.GetNode(i)->release_time <= time + 0.001 &&
                            real.GetNode(i)->type == NODE_TYPE_PICKUP &&
                            real.GetNode(i)->tw_end + Parameters::GetP() >= std::min(n1.release_time, n1.tw_start)) {
                            has_one = true;
                            break;
                        }
                    if (!has_one) continue; //go to the next stochastic request
                }

                //ignore infeasible requests
                if (is_stacy) {
                    double dd = std::abs(n2.x - depot.x) + std::abs(n2.y - depot.y);
                    dd /= Scenarios::max_distance_from_depot;
                    dd *= 60;

                    //if(std::max(n1.release_time + dd, n2.tw_start) + dd > depot_end_time || n1.release_time + dd > n2.tw_end)
                    //	printf("InfReq Scenario:%d id:%d release:%d tw:%d-%d dist:%.1lf\n", k, j, (int)n1.release_time, (int)n2.tw_start, (int)n2.tw_end, dd);

                    if (std::max(n1.release_time + dd, n2.tw_start) + dd > depot_end_time) {
                        inf_request_count++;
                        continue;
                    }
                    if (n1.release_time + dd > n2.tw_end) {
                        inf_request_count++;
                        continue;
                    }
                }

                n1.id = p->GetNodeCount();
                n2.id = n1.id + 1;
                n1.distID = 1;
                n2.distID = dist_id++;
                n1.del_node_id = n2.id;
                n2.pick_node_id = n1.id;
                n1.is_real = n2.is_real = false;
                p->AddNode(n1);
                p->AddNode(n2);
            }

        //Requests creation
        for (int i = 0; i < p->GetNodeCount(); i++)
            if (p->GetNode(i)->type == NODE_TYPE_PICKUP) {
                Node *n1 = p->GetNode(i);
                Node *n2 = p->GetNode(n1->del_node_id);

                Request<Node> r;
                r.id = p->GetRequestCount();
                n1->req_id = r.id;
                n2->req_id = r.id;
                r.AddNode(n1);
                r.AddNode(n2);
                p->AddRequest(r);

                //if(k == 0 && n1->is_real)
                {
                    //double dd = std::abs(n2->x - depot.x) + std::abs(n2->y - depot.y);
                    //dd /= Scenarios::max_distance_from_depot;
                    //dd *= 60;
                    //printf("Real id:%d release:%d tw:%d-%d ", n1->req_id, (int)n1->release_time, (int)n1->tw_start, (int)n1->tw_end);
                    //printf("tw:%d-%d dist:%.1lf\n",(int)n2->tw_start, (int)n2->tw_end, dd);
                }
            }


        int dim = dist_id;
        double **d = new double *[dim];
        double **t = new double *[dim];

        for (int i = 0; i < dim; i++) {
            d[i] = new double[dim];
            t[i] = new double[dim];
            for (int j = 0; j < dim; j++)
                d[i][j] = t[i][j] = 0;
        }

        if (is_stacy) {
            for (int i = 0; i < p->GetNodeCount(); i++) {
                Node *n1 = p->GetNode(i);
                for (int j = 0; j < p->GetNodeCount(); j++) {
                    if (i == j) continue;

                    Node *n2 = p->GetNode(j);
                    //d[n1->distID][n2->distID] = std::abs(n1->x - depot.x) + std::abs(n1->y - depot.y);
                    d[n1->distID][n2->distID] = std::abs(n1->x - n2->x) + std::abs(n1->y - n2->y);
                }
            }

            for (int i = 0; i < dim; i++)
                for (int j = 0; j < dim; j++) {
                    d[i][j] = d[i][j] / max_distance_from_depot * 60;
                    t[i][j] = d[i][j];
                }
        } else if (is_ulmer) {
            // Euclidean  distances
            for (int i = 0; i < p->GetNodeCount(); i++) {
                Node *n1 = p->GetNode(i);
                for (int j = 0; j < p->GetNodeCount(); j++) {
                    if (i == j) continue;

                    Node *n2 = p->GetNode(j);
                    double dist = std::sqrt(((n1->x - n2->x) * (n1->x - n2->x)) + ((n1->y - n2->y) * (n1->y - n2->y)));
                    dist = std::max(ceil(dist / 140), 1.0);
                    if ((n1->type == NODE_TYPE_START_DEPOT || n1->type == NODE_TYPE_PICKUP ||
                         n1->type == NODE_TYPE_END_DEPOT) &&
                        (n2->type == NODE_TYPE_START_DEPOT || n2->type == NODE_TYPE_PICKUP ||
                         n2->type == NODE_TYPE_END_DEPOT))
                        dist = 0;
                    d[n1->distID][n2->distID] = dist;
                    t[n1->distID][n2->distID] = dist;

                    if (n1->type == NODE_TYPE_PICKUP && n2->type == NODE_TYPE_DROP)
                        t[n1->distID][n2->distID] += service_time_depot;
                    else if (n1->type == NODE_TYPE_DROP)
                        t[n1->distID][n2->distID] += service_time_customer;
                }
            }
            //for (int i = 0; i < dim; i++)
            //{
            //	for (int j = 0; j < dim; j++)
            //		printf("%3d ", (int)t[i][j]);
            //	printf("\n");
            //}
            //exit(1);
        } else {
            // Manhattan distances
            for (int i = 0; i < p->GetCustomerCount(); i++) {
                if (p->GetCustomer(i)->type != NODE_TYPE_DROP) continue;
                Node *n1 = p->GetCustomer(i);
                d[0][n1->distID] = d[n1->distID][0] = std::abs(n1->x - depot.x) + std::abs(n1->y - depot.y);

                for (int j = 0; j < p->GetCustomerCount(); j++) {
                    if (p->GetCustomer(j)->type != NODE_TYPE_DROP) continue;
                    Node *n2 = p->GetCustomer(j);
                    d[n1->distID][n2->distID] = std::abs(n1->x - n2->x) + std::abs(n1->y - n2->y);
                }
            }

            for (int i = 0; i < dim; i++)
                for (int j = 0; j < dim; j++) {
                    d[i][j] = d[i][j] * 60 / (120 / 2 + 80 / 2);
                    t[i][j] = d[i][j];
                }
        }


        p->SetMaxtrices(d, t, dim);
    }//for each scenario

    //double avg_requests = 0;
    //for(int k=0;k<Parameters::GetScenarioCount();k++)
    //	avg_requests += probs[k].GetCustomerCount()/2;
    //avg_requests/=Parameters::GetScenarioCount();
    //printf("Scenarios:%d AvgRequests:%.1lf InfRequests:%d\n", Parameters::GetScenarioCount(),avg_requests, inf_request_count);
}

void Scenarios::GenerateReal(Prob<Node, Driver> &prob, double time) {
    for (int j = 0; j < Parameters::GetDriverCount(); j++) {
        Node dep1(depot);
        dep1.x = depot.x;
        dep1.y = depot.y;
        dep1.id = prob.GetNodeCount();
        dep1.distID = 0;
        dep1.type = NODE_TYPE_START_DEPOT;
        dep1.release_time = 0;
        dep1.is_real = true;
        dep1.arrival_time = 0;
        dep1.serv_time = 0;
        dep1.tw_start = depot_start_time;
        dep1.tw_end = depot_end_time;
        Node dep2(depot);
        dep2.x = depot.x;
        dep2.y = depot.y;
        dep2.id = dep1.id + 1;
        dep2.distID = 0;
        dep2.type = NODE_TYPE_END_DEPOT;
        dep2.is_real = true;
        dep2.tw_start = depot_start_time;
        dep2.tw_end = depot_end_time;

        Driver d(driver);
        d.capacity = 999999999;// cap;
        d.StartNodeID = dep1.id;
        d.last_done_node_id = dep1.id;
        d.EndNodeID = dep2.id;
        d.id = j;

        prob.AddNode(dep1);
        prob.AddNode(dep2);
        prob.AddDriver(d);
    }

    for (int j = 0; j < real.GetNodeCount(); j++)
        if (real.GetNode(j)->release_time <= time && real.GetNode(j)->type == NODE_TYPE_PICKUP) {
            Node n1 = *real.GetNode(j);
            Node n2 = *real.GetNode(n1.del_node_id);
            n1.id = prob.GetNodeCount();
            n2.id = n1.id + 1;
            n1.del_node_id = n2.id;
            n2.pick_node_id = n1.id;
            prob.AddNode(n1);
            prob.AddNode(n2);
        }


    //Requests creation
    int dist_id = 2;
    for (int i = 0; i < prob.GetNodeCount(); i++)
        if (prob.GetNode(i)->type == NODE_TYPE_PICKUP) {
            Node *n1 = prob.GetNode(i);
            Node *n2 = prob.GetNode(n1->del_node_id);
            n1->distID = 1;
            n2->distID = dist_id++;

            Request<Node> r;
            r.id = prob.GetRequestCount();
            n1->req_id = r.id;
            n2->req_id = r.id;
            r.AddNode(n1); //Add pick up node to request
            r.AddNode(n2); //Add correspondant delivery node to request
            prob.AddRequest(r);
        }
    //for (int i = 0; i < prob.GetNodeCount(); i++)
    //	prob.GetNode(i)->Show();
    //prob.Show();

    int dim = dist_id;
    double **d = new double *[dim];
    double **t = new double *[dim];

    for (int k = 0; k < dim; k++) {
        d[k] = new double[dim];
        t[k] = new double[dim];
    }
    for (int k = 0; k < dim; k++)
        for (int j = 0; j < dim; j++)
            d[k][j] = t[k][j] = 0;


    if (is_stacy) {
        // Manhattan distances
        /*for (int i = 0; i < prob.GetCustomerCount(); i++)
        {
            if (prob.GetCustomer(i)->type != NODE_TYPE_DROP) continue;
            Node* n1 = prob.GetCustomer(i);
            d[0][n1->distID] = d[n1->distID][0] =

            for (int j = 0; j < prob.GetCustomerCount(); j++)
            {
                if (prob.GetCustomer(j)->type != NODE_TYPE_DROP) continue;
                Node* n2 = prob.GetCustomer(j);
                d[n1->distID][n2->distID] = std::abs(n1->x - n2->x) + std::abs(n1->y - n2->y);
            }
        }*/
        for (int i = 0; i < prob.GetNodeCount(); i++) {
            Node *n1 = prob.GetNode(i);
            for (int j = 0; j < prob.GetNodeCount(); j++) {
                if (i == j) continue;

                Node *n2 = prob.GetNode(j);
                //d[n1->distID][n2->distID] = std::abs(n1->x - depot.x) + std::abs(n1->y - depot.y);
                d[n1->distID][n2->distID] = std::abs(n1->x - n2->x) + std::abs(n1->y - n2->y);
            }
        }

        for (int i = 0; i < dim; i++)
            for (int j = 0; j < dim; j++) {
                d[i][j] = d[i][j] / Scenarios::max_distance_from_depot * 60;
                t[i][j] = d[i][j];
            }
    } else if (is_ulmer) {
        // Euclidean  distances
        for (int i = 0; i < prob.GetNodeCount(); i++) {
            Node *n1 = prob.GetNode(i);
            for (int j = 0; j < prob.GetNodeCount(); j++) {
                if (i == j) continue;

                Node *n2 = prob.GetNode(j);
                double dist = std::sqrt(((n1->x - n2->x) * (n1->x - n2->x)) + ((n1->y - n2->y) * (n1->y - n2->y)));
                dist = std::max(ceil(dist / 140), 1.0);
                if ((n1->type == NODE_TYPE_START_DEPOT || n1->type == NODE_TYPE_PICKUP ||
                     n1->type == NODE_TYPE_END_DEPOT) &&
                    (n2->type == NODE_TYPE_START_DEPOT || n2->type == NODE_TYPE_PICKUP ||
                     n2->type == NODE_TYPE_END_DEPOT))
                    dist = 0;
                d[n1->distID][n2->distID] = dist;
                t[n1->distID][n2->distID] = dist;

                if (n1->type == NODE_TYPE_PICKUP && n2->type == NODE_TYPE_DROP)
                    t[n1->distID][n2->distID] += service_time_depot;
                else if (n1->type == NODE_TYPE_DROP)
                    t[n1->distID][n2->distID] += service_time_customer;
            }
        }
        /*for (int i = 0; i < dim; i++)
        {
            for (int j = 0; j < dim; j++)
                printf("%3d ", (int)t[i][j]);
            printf("\n");
        }
        for(int i=0;i<prob.GetNodeCount();i++)
            for(int j=0;j<prob.GetNodeCount();j++)
                for(int k=0;k<prob.GetNodeCount();k++)
                {
                    Node* n1 = prob.GetNode(i);
                    Node* n2 = prob.GetNode(j);
                    Node* n3 = prob.GetNode(k);
                    if(!n3->IsCustomer()) continue;
                    if(t[n1->distID][n2->distID] > t[n1->distID][n3->distID]+t[n3->distID][n2->distID])
                    {
                        printf("i:%d j:%d k:%d t[i][j]:%.1lf t[i][k]:%.1lf t[k][j]:%.1lf\n", i,j,k,t[n1->distID][n2->distID] , t[n1->distID][n3->distID],t[n3->distID][n2->distID]);
                        n1->Show();
                        n2->Show();
                        n3->Show();
                        exit(1);
                    }
                }*/
    } else {
        // Manhattan distances
        for (int i = 0; i < prob.GetCustomerCount(); i++) {
            if (prob.GetCustomer(i)->type != NODE_TYPE_DROP) continue;
            Node *n1 = prob.GetCustomer(i);
            d[0][n1->distID] = d[n1->distID][0] = std::abs(n1->x - depot.x) + std::abs(n1->y - depot.y);

            for (int j = 0; j < prob.GetCustomerCount(); j++) {
                if (prob.GetCustomer(j)->type != NODE_TYPE_DROP) continue;
                Node *n2 = prob.GetCustomer(j);
                d[n1->distID][n2->distID] = std::abs(n1->x - n2->x) + std::abs(n1->y - n2->y);
            }
        }

        for (int i = 0; i < dim; i++)
            for (int j = 0; j < dim; j++) {
                d[i][j] = d[i][j] * 60 / (120 / 2 + 80 / 2);
                t[i][j] = d[i][j];
            }
    }


    /*printf("dim:%d\nDistance\n", dim);
    for(int k = 0; k < dim; k++)
    {
        for(int j = 0; j < dim; j++)
            printf("%.2d ", (int)d[k][j]);
        printf("\n");
    }
    printf("Time\n");
    for(int k = 0; k < dim; k++)
    {
        for(int j = 0; j < dim; j++)
            printf("%.2d ", (int)t[k][j]);
        printf("\n");
    }*/

    prob.SetMaxtrices(d, t, dim);
}

bool Scenarios::CanWait(Node *n) {
    if (!is_stacy) return false;
    if (n->type == NODE_TYPE_PICKUP) {
        printf("Provide a drop to CanWait(Node * n)\n");
        exit(1);
    }

    double dist = 0;
    dist = std::abs(n->x - depot.x) + std::abs(n->y - depot.y);
    dist /= max_distance_from_depot;
    dist *= 60;

    //n->Show();
    //printf("node:%d rel:%d cur:%.1lf dist:%.1lf twstart:%d\n", n->id, (int)n->release_time, Parameters::GetCurrentTime(), dist, (int) n->tw_start);
    if (std::max(n->release_time, Parameters::GetCurrentTime()) + dist < n->tw_start)
        return true;
    else
        return false;
}

bool Scenarios::IsStillFeasible(Node *n) {
    if (n->type == NODE_TYPE_PICKUP) {
        printf("Provide a drop to IsStillFeasible(Node * n)\n");
        exit(1);
    }

    double dist = 0;
    if (is_stacy) {
        dist = std::abs(n->x - depot.x) + std::abs(n->y - depot.y);
        dist = dist / Scenarios::max_distance_from_depot * 60;
    } else if (is_ulmer) {
        dist = std::sqrt(((n->x - depot.x) * (n->x - depot.x)) + ((n->y - depot.y) * (n->y - depot.y)));
        dist /= 140;
        dist += service_time_depot;
    } else
        dist = dist * 60 / (120 / 2 + 80 / 2);

    if (Parameters::GetCurrentTime() + dist > n->tw_end + 0.001)
        return false;
    else
        return true;
}

void Scenarios::GenerateUlmerScenarios(Scenarios &sc, int real_sc_no) {
    sc.is_ulmer = true;
    sc.is_stacy = false;
    sc.real = scenarios[real_sc_no];
    sc.scenarios.reserve(Parameters::GetScenarioCount());
    sc.depot = depot;
    sc.driver = driver;

    for (int i = real_sc_no + 1; i <= real_sc_no + Parameters::GetScenarioCount(); i++)
        sc.scenarios.push_back(scenarios[i % scenarios.size()]);

    for (int i = 0; i < sc.real.GetNodeCount(); i++)
        sc.real.GetNode(i)->is_real = true;

    sc.time_window_type = time_window_type;
    sc.service_time = service_time;
    sc.depot_start_time = depot_start_time;
    sc.depot_end_time = depot_end_time;
    sc.service_time_depot = service_time_depot;
    sc.service_time_customer = service_time_customer;
}

void Scenarios::Show() {
    printf("Real:%d Scenarios:%d\n", real.GetNodeCount(), (int) scenarios.size());
}
