//Not considering Capacity and Time windows constraints.

#include "InsRmvMethodUlmer.h"
#include "../../../lib/mathfunc.h"
#include "../constants.h"
#include <algorithm>
#include "Decision.h"
#include "Parameters.h"

InsRmvMethodUlmer::InsRmvMethodUlmer(Prob<Node, Driver> *prob, CostFunctionSddp &cost_func) : p(prob),
                                                                                              _cost_func(cost_func) {
    _added_noise = mat_func_get_max_smaller_than(p->GetDistances(), p->GetDimension(), INFINITE);
    _added_noise *= 0.025;
    SetNoise(0);
    show_output = false;
}

InsRmvMethodUlmer::~InsRmvMethodUlmer() {}

void InsRmvMethodUlmer::InsertCost(Sol<Node, Driver> &s, Node *pick, Driver *d, Move<Node, Driver, MoveSddp> &mo) {
    class mv {
    public:
        Node *prev_pick;
        Node *prev_drop;
        double cost;

        mv(Node *pp, Node *pd, double c) : prev_pick(pp), prev_drop(pd), cost(c) {}

        bool operator<(const mv &m) const {
            return cost < m.cost;
        }
    };


    mo.IsFeasible = false;
    mo.DeltaCost = INFINITE;
    mo.n = pick;
    mo.to = d;
    Node *drop = s.GetNode(
            pick->del_node_id);//s.GetProb()->GetRequest(pick)->GetDropNode(0);//Get correspondant delivery node from request

    if (pick->is_fixed_pos) {
        printf("Node:%d is fixed. It shouldn't be inserted\n", pick->id);
        pick->Show();
        s.Show();
        Node *nnn = NULL;
        nnn->id = 4;
        exit(1);
    }

    if (pick->is_assigned_to != -2 && pick->is_assigned_to != d->id) return;


    std::vector <mv> moves;

    Node *prev_pick = s.GetNode(d->last_done_node_id == -1 ? d->StartNodeID : d->last_done_node_id);
    while (prev_pick->type != NODE_TYPE_END_DEPOT) {
        Node *next_pick = s.Next[prev_pick->id];
        /*if(next_pick->type == NODE_TYPE_PICKUP && pick->release_time < next_pick->release_time)
        {
            prev_pick = next_pick;
            continue;
        }*/
        if (prev_pick->type == NODE_TYPE_PICKUP && next_pick->type == NODE_TYPE_PICKUP) {
            prev_pick = next_pick;
            continue;
        }

        double pick_arr = prev_pick->departure_time + p->GetTime(prev_pick, pick);
        if (pick->release_time > pick_arr)
            pick_arr = pick->release_time;

        //if(show_output) printf("prev:%d pick_arr:%.2lf\n", prev_pick->id, pick_arr);

        //if the arrival time at the node isn't feasible
        //All the next positions will be infeasible
        if (pick_arr > pick->tw_end + 0.0001) break;

        if (pick->action_type == DECISION_ACTION_WAIT &&
            (prev_pick->action_type == DECISION_ACTION_GO_NOW || next_pick->action_type == DECISION_ACTION_GO_NOW)) {
            prev_pick = next_pick;
            continue;
        }
        if (pick->action_type == DECISION_ACTION_GO_NOW &&
            (next_pick->action_type == DECISION_ACTION_WAIT || next_pick->release_time > Parameters::GetCurrentTime() ||
             pick_arr > Parameters::GetCurrentTime())) {
            prev_pick = next_pick;
            continue;
        }
        if ((prev_pick->action_type == DECISION_ACTION_GO_NOW || next_pick->action_type == DECISION_ACTION_GO_NOW) &&
            pick->release_time > Parameters::GetCurrentTime()) {
            prev_pick = next_pick;
            continue;
        }

        double next_pick_arr = std::max(pick_arr, pick->tw_start) + pick->serv_time;
        if (next_pick_arr < next_pick->release_time && next_pick->type == NODE_TYPE_DROP)
            next_pick_arr = next_pick->release_time;
        next_pick_arr += p->GetTime(pick, next_pick);
        if (next_pick_arr < next_pick->release_time)
            next_pick_arr = next_pick->release_time;

        //if(show_output) printf("\tnext_pick_arr:%.2lf\n", next_pick_arr);
        //if the arrival time at the next node is after its upper tw
        //no need to call s.GetCost(d)
        //Just go to the next node
        if (next_pick_arr > next_pick->tw_end + 0.001) {
            prev_pick = next_pick;
            continue;
        }


        s.InsertAfter(pick, prev_pick);
        Node *prev_drop = pick;
        double prev_drop_arr = pick_arr;
        while (prev_drop->type != NODE_TYPE_END_DEPOT) {
            Node *next_drop = s.Next[prev_drop->id];

            double drop_arr = std::max(prev_drop_arr, prev_drop->tw_start) + prev_drop->serv_time;
            if (drop_arr < drop->release_time)
                drop_arr = drop->release_time;
            drop_arr += p->GetTime(prev_drop, drop);

            //if(show_output) printf("\t\tdrop_arr:%.2lf\n", drop_arr);
            //if the arrival time at the node isn't feasible
            //All the next positions will be infeasible
            if (drop_arr > drop->tw_end + 0.001) {
                if (prev_drop == pick) {
                    s.Remove(pick);
                    return;
                } else
                    break;
            }


            double next_drop_arr = std::max(drop_arr, drop->tw_start) + drop->serv_time;
            if (next_drop->type == NODE_TYPE_PICKUP) {
                next_drop_arr += p->GetTime(drop, next_drop);
                if (next_drop->release_time > next_drop_arr)
                    next_drop_arr = next_drop->release_time;
            } else if (next_drop->type == NODE_TYPE_DROP) {
                if (next_drop->release_time > next_drop_arr)
                    next_drop_arr = next_drop->release_time;
                next_drop_arr += p->GetTime(drop, next_drop);
            } else
                next_drop_arr += p->GetTime(drop, next_drop);

            //if(show_output) printf("\t\tnext_drop_arr:%.2lf\n", next_drop_arr);


            double new_prev_drop_arr = std::max(prev_drop_arr, prev_drop->tw_start) + prev_drop->serv_time;
            if (next_drop->type == NODE_TYPE_PICKUP) {
                new_prev_drop_arr += p->GetTime(prev_drop, next_drop);
                if (next_drop->release_time > new_prev_drop_arr)
                    new_prev_drop_arr = next_drop->release_time;
            } else if (next_drop->type == NODE_TYPE_DROP) {
                if (next_drop->release_time > new_prev_drop_arr)
                    new_prev_drop_arr = next_drop->release_time;
                new_prev_drop_arr += p->GetTime(prev_drop, next_drop);
            }

            //if the arrival time at the next node is after its upper tw
            //no need to call s.GetCost(d)
            //Just go to the next node
            if (next_drop_arr > next_drop->tw_end + 0.001) {
                prev_drop_arr = new_prev_drop_arr;
                prev_drop = next_drop;
                continue;
            }

            if (prev_drop->type == NODE_TYPE_PICKUP && prev_drop != pick && next_drop->type == NODE_TYPE_PICKUP) {
                prev_drop_arr = new_prev_drop_arr;
                prev_drop = next_drop;
                continue;
            }

            double deltaDist = 0;
            if (prev_drop == pick)
                deltaDist = p->GetDist(prev_pick, pick) +
                            p->GetDist(pick, drop) +
                            p->GetDist(drop, next_drop) -
                            p->GetDist(prev_pick, next_drop);
            else
                deltaDist = p->GetDist(prev_pick, pick) +
                            p->GetDist(pick, next_pick) -
                            p->GetDist(prev_pick, next_pick) +
                            p->GetDist(prev_drop, drop) +
                            p->GetDist(drop, next_drop) -
                            p->GetDist(prev_drop, next_drop);
            double newcost = deltaDist + GetNoise() * (2 * mat_func_get_rand_double() * _added_noise - _added_noise);
            if (newcost >= mo.DeltaCost) {
                prev_drop_arr = new_prev_drop_arr;
                prev_drop = next_drop;
                continue;
            }


            /*s.InsertAfter(drop, prev_drop);
            double cost = _cost_func.GetCost(s,d);
            s.Remove(drop);
            if(cost < BIG_DOUBLE)
            {
                mo.DeltaCost = newcost;
                mo.IsFeasible = true;
                mo.move.prev_pick = prev_pick;
                mo.move.prev_drop = prev_drop;
            }*/
            moves.push_back(mv(prev_pick, prev_drop, newcost));

            prev_drop_arr = new_prev_drop_arr;
            prev_drop = next_drop;
        }
        s.Remove(pick);
        prev_pick = next_pick;
    }


    //printf("moves:%d\n", (int)moves.size());

    std::sort(moves.begin(), moves.end());
    //for(size_t i=0;i<std::min(20, (int)moves.size());i++)
    for (size_t i = 0; i < moves.size(); i++) {
        //if(moves.size() >= 20)
        //	printf("move:%d cost:%.2lf\n", (int)i, moves[i].cost);
        if (i >= 1 && moves[i - 1].cost == moves[i].cost) continue;

        s.InsertAfter(pick, moves[i].prev_pick);
        s.InsertAfter(drop, moves[i].prev_drop);
        double cost = _cost_func.GetCost(s, d);
        s.Remove(pick);
        s.Remove(drop);
        if (cost < BIG_DOUBLE) {
            mo.DeltaCost = moves[i].cost;
            mo.IsFeasible = true;
            mo.move.prev_pick = moves[i].prev_pick;
            mo.move.prev_drop = moves[i].prev_drop;
            return;
        }
    }
    //if(moves.size() >= 20)
    //	exit(1);
}

void InsRmvMethodUlmer::ApplyInsertMove(Sol<Node, Driver> &s, Move<Node, Driver, MoveSddp> &m) {
    Node *drop = s.GetProb()->GetRequest(m.n)->GetNode(1);
    if (m.from != NULL) {
        s.Remove(m.n); //Remove Pickup node
        s.Remove(drop); //Remove correspondant delivery node
        m.from->sum_demand -= m.n->demand + drop->demand;
    } else {
        s.RemoveFromUnassigneds(m.n);
        s.RemoveFromUnassigneds(drop);
    }
    s.InsertAfter(m.n, m.move.prev_pick);
    s.InsertAfter(drop, m.move.prev_drop);
}

void InsRmvMethodUlmer::RemoveCost(Sol<Node, Driver> &s, Node *pick, Move<Node, Driver, MoveSddp> &m) {
    Node *drop = s.GetNode(pick->del_node_id);
    m.IsFeasible = true;
    m.n = pick;
    m.to = NULL;
    m.move.prev_pick = NULL;
    m.move.prev_drop = NULL;
    m.from = s.GetAssignedTo(pick);
    double deltaDist = 0;
    if (m.from != NULL) {
        m.move.prev_pick = s.GetPrev(pick);
        m.move.prev_drop = s.GetPrev(drop);
        if (m.move.prev_drop == pick)
            deltaDist = -p->GetDist(m.move.prev_pick, pick) -
                        p->GetDist(pick, drop) -
                        p->GetDist(drop, s.GetNext(drop)) +
                        p->GetDist(m.move.prev_pick, s.GetNext(drop));
        else
            deltaDist = -p->GetDist(m.move.prev_pick, pick) -
                        p->GetDist(pick, s.GetNext(pick)) -
                        p->GetDist(m.move.prev_drop, drop) -
                        p->GetDist(drop, s.GetNext(drop)) +
                        p->GetDist(m.move.prev_pick, s.GetNext(pick)) +
                        p->GetDist(m.move.prev_drop, s.GetNext(drop));

    }

    m.DeltaCost = m.DeltaDistance = deltaDist;
}
