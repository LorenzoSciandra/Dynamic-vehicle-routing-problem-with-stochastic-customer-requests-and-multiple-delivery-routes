#include "ExchangeSddp.h"
#include "../../../lib/mathfunc.h"

void ExchangeSddp::Optimize(Sol<Node, Driver> &s) {
    //printf("Exchange SolCost:%.3lf\n", s.GetCost());
    _insrmv->SetNoise(0);

    int move_count = 0;
    bool found = true;
    while (found) {
        found = false;

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
            Request<Node> *r1 = requests[i];
            Driver *d1 = s.GetAssignedTo(r1);

            //double c1 = s.GetCost(d1);
            Move<Node, Driver, MoveSddp> r1_rem;
            _insrmv->RemoveCost(s, r1, r1_rem);
            if (d1 != NULL) {
                s.Remove(r1);
                s.Update(d1);
            }

            //double c2 = s.Update(d1);
            //printf("req:%d rmv:%.2lf old:%.2lf new:%.2lf\n",r1->id, r1_rem.DeltaCost, c1,c2);


            bool found_improv = false;
            for (size_t j = i + 1; j < requests.size(); j++) {
                Request<Node> *r2 = requests[j];
                Driver *d2 = s.GetAssignedTo(r2);
                if (d1 == d2) continue;

                Move<Node, Driver, MoveSddp> r2_rem;
                _insrmv->RemoveCost(s, r2, r2_rem);
                if (d2 != NULL) {
                    s.Remove(r2);
                    s.Update(d2);
                }

                Move<Node, Driver, MoveSddp> r1_ins;
                Move<Node, Driver, MoveSddp> r2_ins;

                if (d2 != NULL) _insrmv->InsertCost(s, r1, d2, r1_ins); else r1_ins.DeltaCost = 0;
                if (d1 != NULL) _insrmv->InsertCost(s, r2, d1, r2_ins); else r2_ins.DeltaCost = 0;


                if ((r2_rem.DeltaCost + r1_rem.DeltaCost + r1_ins.DeltaCost + r2_ins.DeltaCost < -0.0001 &&
                     !(r1->GetNode(0)->is_real && d2 == NULL) && !(r2->GetNode(0)->is_real && d1 == NULL)) ||
                    (d1 == NULL && r1->GetNode(0)->is_real && !r2->GetNode(0)->is_real && r1_ins.IsFeasible) ||
                    (d2 == NULL && r2->GetNode(0)->is_real && !r1->GetNode(0)->is_real && r2_ins.IsFeasible)) {


                    if (d1 != NULL) _insrmv->ApplyInsertMove(s, r2_ins);
                    if (d2 != NULL) _insrmv->ApplyInsertMove(s, r1_ins);

                    if (d1 != NULL) s.Update(d1);
                    if (d2 != NULL) s.Update(d2);

                    //printf("r1:%d r2:%d ", r1->id,r2->id);
                    //printf("r1ins:%lf r2ins:%lf ", r1_ins.DeltaCost, r2_ins.DeltaCost);
                    //printf("r1rmv:%lf r2rmv:%lf ", r1_rem.DeltaCost, r2_rem.DeltaCost);
                    //printf("sum:%lf\n", r2_rem.DeltaCost+r1_rem.DeltaCost+r1_ins.DeltaCost+r2_ins.DeltaCost);
                    //if(move_count % 1000 == 0)
                    //printf("cnt:%d r1:%d d1:%d r2:%d d2:%d cost:%lf\n",move_count,r1->id,d1!=NULL?d1->id:-1,r2->id,d2!=NULL?d2->id:-1,s.GetCost());

                    move_count++;
                    found = true;
                    found_improv = true;
                    break;
                } else {
                    if (d2 != NULL) {
                        r2_rem.to = r2_rem.from;
                        r2_rem.from = NULL;
                        _insrmv->ApplyInsertMove(s, r2_rem);
                        s.Update(d2);
                    }
                }
            }//end for each neighbor


            if (!found_improv && d1 != NULL) {
                r1_rem.to = r1_rem.from;
                r1_rem.from = NULL;
                _insrmv->ApplyInsertMove(s, r1_rem);
                s.Update(d1);
            }


        }//end for each node





    }//end while

    //printf("Final Cost:%.3lf\n", s.GetCost());
}
