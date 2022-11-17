
#ifndef DECISIONMULTISET_H
#define DECISIONMULTISET_H

#include "Decisions.h"
#include "DecisionsGroup.h"
#include "Scenarios.h"
#include <float.h>
#include <vector>
#include <stdio.h>

class DecisionMultiSet {

public:
    void Add(Decisions &d) { reports.push_back(d); }

    void Add(Decisions *d) { reports.push_back(*d); }

    Decisions *GetDecisions(int i) { return &reports[i]; }

    void ReplaceDecisions(int i, Decisions &d) { reports[i] = d; }

    void ReplaceDecisions(int i, Decisions *d) { reports[i] = *d; }

    void GetConsensusByDrivers(Decisions &decisions);

    void GetConsensusStacy(Decisions &decisions);

    void GetConsensusMinimalDistance(Decisions &decisions);

    int GetConsensusLevel();


    void GetNextAssignmentDecisions(Decisions &taken_decisions, Decisions &decisions, Scenarios &sc);

    void GetNextAssignmentDecisions2(Decisions &taken_decisions, Decisions &decisions, Scenarios &sc);

    void GetNextActionDecisions(Decisions &taken_decisions, Decisions &decisions, Scenarios &sc);
    //void GetNextTwDecisions(Decisions & taken_decisions, Decisions &decisions, Scenarios & sc);

    void GetMajorityActionDecisions(Decisions &taken_decisions, Decisions &decisions, Scenarios &sc);

    void Clear() { reports.clear(); }

    void Show();

    void ShowOnlyRealCustomer();

    void ShowOnlyForRequest(int req_id);

    void ShowOnlyForDriver(int driver_id);

    //void ShowArrivals();
    int GetReportCount() { return (int) reports.size(); }

    double GetAverageCost() {
        if(reports.size()==0){
            return DBL_MAX;
        }
        double c = 0;
        for (size_t i = 0; i < reports.size(); i++) c += reports[i].cost;
        return c / reports.size();
    }

    double GetAverageDist() {
        double c = 0;
        for (size_t i = 0; i < reports.size(); i++) c += reports[i].distances;
        return c / reports.size();
    }

    double GetAverageUn() {
        double c = 0;
        for (size_t i = 0; i < reports.size(); i++) c += reports[i].nb_unassigneds;
        return c / reports.size();
    }

    double GetAverageRealUn() {
        double c = 0;
        for (size_t i = 0; i < reports.size(); i++) c += reports[i].nb_real_unassigneds;
        return c / reports.size();
    }

    bool GetNextDecisionProgressive(Decisions &decisions, Decision &decision);

private:
    std::vector <Decisions> reports;
};

#endif
