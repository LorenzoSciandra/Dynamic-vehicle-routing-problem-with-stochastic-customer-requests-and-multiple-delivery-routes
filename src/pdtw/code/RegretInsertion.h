/*
 * Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca
 * If you have improvements, please contact me!
 */


#ifndef REGRET_INSERTION
#define REGRET_INSERTION

#include "Solution.h"
#include "Move.h"
#include "InsRmvMethod.h"
#include "../../lib/mathfunc.h"
#include "InsertOperator.h"
#include <vector>
#include <list>


template<class NodeT, class DriverT, class MoveT>
struct RegretMoveSorter {
    bool operator()(Move<NodeT, DriverT, MoveT> *n1, Move<NodeT, DriverT, MoveT> *n2) {
        return (n1->DeltaCost < n2->DeltaCost);
    }
};

template<class NodeT, class DriverT, class MoveT>
class RegretInsertion : public InsertOperator<NodeT, DriverT> {

public:
    RegretInsertion(Prob<NodeT, DriverT> &pr, InsRmvMethod<NodeT, DriverT, MoveT> *insrmv) :
            move_vect(0), _insrmv(insrmv), _moves(0), _used_k(2), _k_regret(2) {
        _moves.resize(pr.GetRequestCount());
        for (int i = 0; i < pr.GetRequestCount(); i++)
            _moves[i].resize(pr.GetDriverCount());
    }

    void SetK(int k) { _k_regret = k; }

    void SetNoise(double noise) { _insrmv->SetNoise(noise); }

    void Insert(Sol<NodeT, DriverT> &s) {
        _used_k = MIN_INT(s.GetDriverCount(), _k_regret);

        if ((int) _moves[0].size() < s.GetDriverCount())
            for (int i = 0; i < s.GetRequestCount(); i++)
                _moves[i].resize(s.GetDriverCount());

        typedef typename std::list<Request<NodeT> *>::iterator list_req_iterator;
        std::list < Request<NodeT> * > to_insert;
        s.Update();

        for (int i = 0; i < s.GetUnassignedCount(); i++) {
            NodeT *n = s.GetUnassigned(i);
            Request<NodeT> *r = s.GetProb()->GetRequest(n);

            if (n == r->GetParent()) {
                to_insert.push_back(r);
                for (int j = 0; j < s.GetDriverCount(); j++) {
                    DriverT *d = s.GetDriver(j);
                    _insrmv->InsertCost(s, n, d, _moves[r->id][d->id]);
                }
            }
        }

        while (to_insert.size() > 0) {
            //printf("\nIteration:%zu\n",to_insert.size());
            Move<NodeT, DriverT, MoveT> best;
            double maxRegret = -INFINITE;
            list_req_iterator best_it;

            for (list_req_iterator it = to_insert.begin(); it != to_insert.end();) {
                Move<NodeT, DriverT, MoveT> m;
                Request<NodeT> *r = *it;
                double regret = GetRegretCost(s, r, m);

                if (m.IsFeasible && (regret > maxRegret || (regret == maxRegret && m.DeltaCost < best.DeltaCost))) {
                    maxRegret = regret;
                    best = m;
                    best_it = it;
                    it++;
                } else if (!m.IsFeasible)
                    it = to_insert.erase(it);
                else
                    it++;
            }

            if (best.IsFeasible) {
                //Request<NodeT> * r = s.GetProb()->GetRequest(best.n);
                //Move<NodeT, DriverT, MoveT> m;
                //_insrmv->InsertCost(s, r, best.to, m);
                //printf("Count:%d n:%d Regret:%lf drv:%d fea:%d\n",s.GetUnassignedCount(), best.n->id, maxRegret, best.to->id, best.IsFeasible);

                best.from = NULL;
                _insrmv->ApplyInsertMove(s, best);
                //printf("mcost:%lf cost:%lf\n",m.DeltaCost, s.GetCost(best.to));
                s.Update(best.to);
                //printf("Update\n");
                to_insert.erase(best_it);
            }

            for (list_req_iterator it = to_insert.begin(); it != to_insert.end(); it++) {
                Request<NodeT> *r = *it;
                if (_moves[r->id][best.to->id].IsFeasible)
                    _insrmv->InsertCost(s, r, best.to, _moves[r->id][best.to->id]);
            }
            //s.Show();
        }//end while
    }

    double GetRegretCost(Sol<NodeT, DriverT> &s, Request<NodeT> *r, Move<NodeT, DriverT, MoveT> &m) {
        int nbfeasible = 0;
        move_vect.clear();
        for (int j = 0; j < s.GetDriverCount(); j++) {
            move_vect.push_back(&_moves[r->id][s.GetDriver(j)->id]);
            if (_moves[r->id][s.GetDriver(j)->id].IsFeasible)
                nbfeasible++;
        }
        partial_sort(move_vect.begin(), move_vect.begin() + _used_k, move_vect.end(), move_sorter);
        double cost = 0;
        m = *move_vect[0];
        for (int j = 1; j < _used_k; j++)
            cost += move_vect[j]->DeltaCost - move_vect[0]->DeltaCost;
        //printf("Request:%d nbfeasible:%d cost:%.2lf fea:%d\n", r->id,nbfeasible, cost, (int)m.IsFeasible);
        return cost;
    }

private:

    std::vector<Move<NodeT, DriverT, MoveT> *> move_vect;
    InsRmvMethod<NodeT, DriverT, MoveT> *_insrmv;
    std::vector <std::vector<Move<NodeT, DriverT, MoveT> >> _moves; //[nodes][driver]

    int _used_k;
    int _k_regret;

    RegretMoveSorter<NodeT, DriverT, MoveT> move_sorter;
};


#endif
