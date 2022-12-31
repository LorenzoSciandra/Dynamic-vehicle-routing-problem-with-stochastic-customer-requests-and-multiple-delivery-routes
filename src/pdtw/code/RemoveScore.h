/*
 * Copyright Jean-Francois Cote 2012
 * 
 * The code may be used for academic, non-commercial purposes only.
 * 
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
 *
*/




#ifndef REMOVE_SCORE
#define REMOVE_SCORE

#include "Solution.h"
#include "../../lib/mathfunc.h"
#include "RemoveOperator.h"
#include "Relatedness.h"
#include <vector>
#include <algorithm>

template<class NodeT, class DriverT>
struct RemoveScoreSorter {
    Sol<NodeT, DriverT> &_sol;
    Relatedness<NodeT, DriverT> *rel;

    RemoveScoreSorter(Sol<NodeT, DriverT> &s) : _sol(s) {}

    bool operator()(NodeT *n1, NodeT *n2) {
        return (rel->GetScore(_sol, n1) < rel->GetScore(_sol, n2));
    }
};

template<class NodeT, class DriverT>
class RemoveScore : public RemoveOperator<NodeT, DriverT> {
public:
    RemoveScore(Relatedness<NodeT, DriverT> *rel) : relatedness(rel) {}

    ~RemoveScore() {}

    void Remove(Sol<NodeT, DriverT> &s, int count) {
        vect.clear();
        for (int i = 0; i < s.GetCustomerCount(); i++) {
            if (s.GetAssignedTo(s.GetCustomer(i)) != NULL)
                vect.push_back(s.GetCustomer(i));
        }
        if (vect.size() <= 1) return;
        int size = vect.size();

        RemoveScoreSorter<NodeT, DriverT> mysorter(s);
        mysorter.rel = relatedness;

        int cpt = MIN_INT(count, (int) vect.size());
        for (int i = 0; i <= cpt; i++) {
            sort(vect.begin(), vect.begin() + size, mysorter);
            int index = mat_func_get_rand_int(0, size);
            s.RemoveAndUnassign(vect[index]);
            vect[index] = vect[size - 1];
            size--;
        }

    }

private:
    std::vector<NodeT *> vect;
    Relatedness<NodeT, DriverT> *relatedness;
};


#endif
