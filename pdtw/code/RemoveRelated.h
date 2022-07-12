/*
 * Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca
 * If you have improvements, please contact me!
 */

#include "Request.h"
#include "Solution.h"
#include "../../lib/mathfunc.h"
#include "RemoveOperator.h"
#include "Relatedness.h"
#include <vector>
#include <algorithm>
#include <math.h>

template<class NodeT, class DriverT>
struct RemoveRelatedSorter {
    NodeT *selectedNode;
    Relatedness<NodeT, DriverT> *rel;

    bool operator()(Request<NodeT> *r1, Request<NodeT> *r2) {
        return (rel->GetRelatedness(r1->GetParent(), selectedNode) <
                rel->GetRelatedness(r2->GetParent(), selectedNode));
    }
};

template<class NodeT, class DriverT>
class RemoveRelated : public RemoveOperator<NodeT, DriverT> {
public:
    RemoveRelated(Relatedness<NodeT, DriverT> *rel) : relatedness(rel) {}

    ~RemoveRelated() {}


    void Remove(Sol<NodeT, DriverT> &s, int count) {
        requests.clear();
        removed.clear();
        for (int i = 0; i < s.GetRequestCount(); i++) {
            Request<NodeT> *r = s.GetRequest(i);
            if (s.GetAssignedTo(r->GetParent()) != NULL)
                requests.push_back(r);
            else
                removed.push_back(r);
        }
        if (requests.size() <= 1) return;
        int size = requests.size() - 1;
        int index = mat_func_get_rand_int(0, requests.size());

        s.RemoveAndUnassign(requests[index]);

        removed.push_back(requests[index]);
        requests[index] = requests[size];
        //size--;

        RemoveRelatedSorter<NodeT, DriverT> mysorter;
        mysorter.rel = relatedness;

        int cpt = MIN_INT(count - 1, (int) requests.size() - 2);
        for (int i = 0; i < cpt; i++) {
            index = mat_func_get_rand_int(0, removed.size());
            mysorter.selectedNode = removed[index]->GetParent();
            sort(requests.begin(), requests.begin() + size, mysorter);

            double pRelated = 6; //The value is set equal to that chosen by Ropke and Pisinger. Note that it might also be a random number
            index = (int) size * pow(mat_func_get_rand_double(), pRelated);

            s.RemoveAndUnassign(requests[index]);

            removed.push_back(requests[index]);
            requests[index] = requests[size - 1];
            size--;
        }
    }

    void SetRelatednessFunction(Relatedness<NodeT, DriverT> *r) {
        relatedness = r;
    }

private:
    std::vector<Request<NodeT> *> requests;
    std::vector<Request<NodeT> *> removed;
    Relatedness<NodeT, DriverT> *relatedness;
};
