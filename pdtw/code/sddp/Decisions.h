#ifndef DECISIONS_H
#define DECISIONS_H

#include "Decision.h"
#include "Parameters.h"
#include <vector>
#include <map>

class Decisions {
public:

    void Add(Decision &d) { decisions.push_back(d); }

    void Add(Decision *d) { Add(*d); }

    void Add(Decisions &decs) { for (int i = 0; i < decs.GetCount(); i++)Add(decs.Get(i)); }

    void Add(Decisions *decs) { for (int i = 0; i < decs->GetCount(); i++)Add(decs->Get(i)); }

    void AddAll(std::vector<Decision> decs) {
        for (const auto &item: decs) {
            decs.push_back(item);
        }
    }

    int GetCount() { return (int) decisions.size(); }

    int GetUnassignedCount();

    bool HasDecisionReqDriver(int req_id, int driver_id);

    bool HasReqBeenPerformed(int req_id);

    bool HasDecisionReal(int req_id);

    bool HasAssignmentDecisionReq(int req_id);

    bool HasAssignmentDecisionReq(int req_id, int action_type, int driver_id);

    bool HasActionDecisionReq(int req_id);

    bool HasActionDecisionReqExceptDontUnassign(int req_id);

    Decision *GetByRequestID(int req_id);

    bool HasDecisionCharacteristics(Decision *d);

    Decision *Get(int i) { return &decisions[i]; }

    void FillDecisionsByDrivers();

    int GetDriverDecisionCount(int drv) { return (int) drivers_decisions[drv].size(); }

    Decision *GetDriverDecision(int drv, int i) { return drivers_decisions[drv][i]; }

    bool IsDriverAtDepot(int drv);

    bool HasDriverAtDepot();

    void Clear() { decisions.clear(); }

    void Remove(double time);

    void pop_back() { decisions.pop_back(); }

    void Show();

    void ShowOnlyReal();

    void ShowOnlyRealCustomer();

    void ShowOnlyForRequest(int req_id);

    void ShowByDriver(int id);

    void ShowByAssignments();

    void Sort();

    void FillMap(std::map<int, Decision *> &map_dec, int type);

    double GetNextEvent();

    std::vector<Decision> GetRealDecisions() {
        std::vector<Decision> real_decisions;
        for (const auto &item: decisions) {
            if (item.is_real) {
                real_decisions.push_back(item);
            }
        }

        return real_decisions;
    };

    double cost;
    int nb_unassigneds;
    int nb_real_unassigneds;
    double distances;
private:
    std::vector<Decision> decisions;
    std::vector<std::vector<Decision *>> drivers_decisions;
};

#endif
