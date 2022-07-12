
#include "RelocateSddp.h"
#include "../../../lib/mathfunc.h"
#include "Parameters.h"

void RelocateSddp::Optimize(Sol<Node, Driver> &s) {
    _insrmv->SetNoise(0);
    s.Update();
    //printf("Relocate Optimize Cost:%.3lf\n", s.GetCost());
    bool found = true;
    while (found) {
        found = false;
        double oldcost = s.GetCost();

        requests.clear();
        for (int i = 0; i < s.GetRequestCount(); i++)
            if (!s.GetRequest(i)->GetNode(0)->IsFixed())
                requests.push_back(s.GetRequest(i));

        if (requests.size() >= 3)
            for (int i = (int) requests.size() - 1; i >= 1; i--) {
                int index = mat_func_get_rand_int(0, i);
                Request<Node> *r = requests[i];
                requests[i] = requests[index];
                requests[index] = r;
            }

        for (size_t i = 0; i < requests.size(); i++) {
            //printf("Req:%d\n", requests[i]->id);
            Move<Node, Driver, MoveSddp> rmv;
            _insrmv->RemoveCost(s, requests[i], rmv);
            if (s.GetAssignedTo(requests[i]) != NULL) {
                //s.Show(s.GetAssignedTo( requests[i] ));
                s.Remove(requests[i]);
            } else
                rmv.DeltaCost = -INFINITE;

            Move<Node, Driver, MoveSddp> best;
            for (int j = 0; j < s.GetDriverCount(); j++) {
                Driver *d = s.GetDriver(j);
                if (d == rmv.from) continue;
                //printf("Calc req:%d to:%d\n", requests[i]->id, d->id);
                //s.Show();
                Move<Node, Driver, MoveSddp> tmp;
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
