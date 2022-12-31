/*
 * Copyright Jean-Francois Cote 2016
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca
 * If you have improvements, please contact me!
 */

#ifndef SEQUENTIAL_INSERTION
#define SEQUENTIAL_INSERTION

#include "Solution.h"
#include "Move.h"
#include "InsRmvMethod.h"
#include "../../lib/mathfunc.h"
#include "InsertOperator.h"

template<class NodeT, class DriverT, class MoveT>
class SeqInsertion : public InsertOperator<NodeT, DriverT> {
public:
    SeqInsertion(InsRmvMethod<NodeT, DriverT, MoveT> *insrmv) : _insrmv(insrmv) {}


    void Insert(Sol<NodeT, DriverT> &s) {
        s.Update();
        std::vector < Request<NodeT> * > refused;
        while (s.GetUnassignedCount() > 0) {
            NodeT *n = s.GetUnassigned(0);
            Request<NodeT> *r = s.GetProb()->GetRequest(n);

            Move<NodeT, DriverT, MoveT> best;
            best.IsFeasible = false;
            best.DeltaCost = INFINITE;

            for (int j = 0; j < s.GetDriverCount(); j++) {
                Move<NodeT, DriverT, MoveT> m;
                _insrmv->InsertCost(s, r, s.GetDriver(j), m);
                if (m.IsFeasible && m.DeltaCost < best.DeltaCost)
                    best = m;
            }

            best.from = NULL;
            if (best.IsFeasible) {
                _insrmv->ApplyInsertMove(s, best);
                s.Update(best.to);
            } else {
                refused.push_back(r);
                s.RemoveFromUnassigneds(r);
            }
        }

        for (size_t i = 0; i < refused.size(); i++)
            s.AddToUnassigneds(refused[i]);
    }

private:
    InsRmvMethod<NodeT, DriverT, MoveT> *_insrmv;

};


#endif
