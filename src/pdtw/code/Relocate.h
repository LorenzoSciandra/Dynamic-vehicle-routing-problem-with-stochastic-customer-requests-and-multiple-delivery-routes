/*
* Copyright Jean-Francois Cote 2014
*
* The code may be used for academic, non-commercial purposes only.
*
* Please contact me at cotejean@iro.umontreal.ca for questions
*
* If you have improvements, please contact me!
*
*/
#ifndef _RELOCATE_H
#define _RELOCATE_H

#include "Solution.h"
#include "IAlgorithm.h"
#include "InsRmvMethod.h"
#include "../../lib/mathfunc.h"
#include <vector>
#include <algorithm>


template<class NodeT, class DriverT, class MoveT>
class Relocate : public IAlgorithm<NodeT, DriverT> {

public:
    Relocate(InsRmvMethod<NodeT, DriverT, MoveT> *insrmv) : _insrmv(insrmv) {}

    void Optimize(Sol<NodeT, DriverT> &s, ISolutionList<NodeT, DriverT> *list) {
        Optimize(s);
        if (list != NULL && s.IsFeasible())
            list->Add(s);
    }

    void Optimize(Sol<NodeT, DriverT> &s) {
        _insrmv->SetNoise(0);
        s.Update();
        //printf("Relocate Optimize Cost:%.3lf\n", s.GetCost());
        bool found = true;
        while (found) {
            found = false;
            double oldcost = s.GetCost();

            requests.clear();
            for (int i = 0; i < s.GetRequestCount(); i++)
                requests.push_back(s.GetRequest(i));

            for (int i = 0; i < s.GetRequestCount(); i++) {
                int index = mat_func_get_rand_int(0, s.GetRequestCount());
                Request<NodeT> *r = requests[i];
                requests[i] = requests[index];
                requests[index] = r;
            }

            for (size_t i = 0; i < requests.size(); i++) {
                //printf("Req:%d\n", requests[i]->id);
                Move<NodeT, DriverT, MoveT> rmv;
                _insrmv->RemoveCost(s, requests[i], rmv);
                if (s.GetAssignedTo(requests[i]) != NULL) {
                    //s.Show(s.GetAssignedTo( requests[i] ));
                    s.Remove(requests[i]);
                } else
                    rmv.DeltaCost = -INFINITE;

                Move<NodeT, DriverT, MoveT> best;
                for (int j = 0; j < s.GetDriverCount(); j++) {
                    DriverT *d = s.GetDriver(j);
                    if (d == rmv.from) continue;
                    //printf("Calc req:%d to:%d\n", requests[i]->id, d->id);
                    //s.Show();
                    Move<NodeT, DriverT, MoveT> tmp;
                    _insrmv->InsertCost(s, requests[i], d, tmp);
                    if (tmp.DeltaCost + 0.0001 < best.DeltaCost)
                        best = tmp;
                }

                if (rmv.DeltaCost + best.DeltaCost < -0.0001 && best.IsFeasible)
                    //if(best.IsFeasible)
                {
                    //printf("Relocating req:%d from:%d to:%d cost:%.2lf\n", requests[i]->id, rmv.from->id, best.to->id, rmv.DeltaCost + best.DeltaCost);
                    if (rmv.from != NULL) {
                        //s.Remove(requests[i]); //JF : no need to remove because it was removed first
                        s.Update(rmv.from);
                    }
                    _insrmv->ApplyInsertMove(s, best);
                    s.Update(best.to);
                    found = true;
                    //printf("n:%d rmv:%.2lf best:%.2lf from:%d to:%d SolCost:%.3lf\n",requests[i]->no,rmv.DeltaCost, best.DeltaCost,rmv.from->id,best.to->id,s.GetCost());
                    //s.Show();
                } else {
                    if (rmv.from != NULL) {
                        rmv.to = rmv.from;
                        rmv.from = NULL;
                        _insrmv->ApplyInsertMove(s, rmv);
                        s.Update(rmv.to);
                    }
                }
            }
            if (oldcost - 0.01 <= s.GetCost()) break;
        }//end while
        //printf("Final Cost:%.3lf\n", s.GetCost());
    }


private:
    InsRmvMethod<NodeT, DriverT, MoveT> *_insrmv;
    std::vector<Request<NodeT> *> requests;
};


#endif
