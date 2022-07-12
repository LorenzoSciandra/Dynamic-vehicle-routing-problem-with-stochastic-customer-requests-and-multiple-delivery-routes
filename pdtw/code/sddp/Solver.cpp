#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "NodeSddp.h"
#include "DriverSddp.h"
#include "../ProblemDefinition.h"
#include "../Solution.h"
#include "../SequentialInsertion.h"
#include "RemoveRandomSddp.h"
#include "RemoveRelatedQuickSddp.h"

#include "../RegretInsertion.h"
#include "../RegretInsertionOperator.h"
#include "../ALNS.h"


#include "InsRmvMethodSddp.h"
#include "CostFunctionSddp.h"
#include "RelatednessSddp.h"
#include "Scenario.h"
#include "Scenarios.h"
#include "Solver.h"
#include "Parameters.h"
#include "Decision.h"
#include "Decisions.h"

#include "ExchangeSddp.h"
#include "RelocateSddp.h"
#include "Opt2.h"

using namespace std;

Solver::Solver(Prob<Node, Driver> &pr) : prob(pr), cost_func(&pr), method(&pr, cost_func), sol(&pr, &cost_func) {
    sol.PutAllNodesToUnassigned();
    alns_iteration_count = Parameters::GetAlnsIterations();
}

void Solver::Optimize() {
    //printf("Solver::Optimize()\n");
    //prob.Show();
    //sol.Show();
    if (prob.GetCustomerCount() == 0) {
        cost = distances = 0;
        nb_unassigneds = 0;
        return;
    }
    SeqInsertion<Node, Driver, MoveSddp> seq(&method);
    RegretInsertion<Node, Driver, MoveSddp> regret(prob, &method);
    //RegretInsertionOperator<Node, Driver, MoveSddp> regret2(&regret, 3, 0);
    //RegretInsertionOperator<Node, Driver, MoveSddp> regret2n(&regret, 3, 1);
    RegretInsertionOperator<Node, Driver, MoveSddp> regretk(&regret, prob.GetDriverCount(), 0);
    RegretInsertionOperator<Node, Driver, MoveSddp> regretkn(&regret, prob.GetDriverCount(), 1);

    regretkn.Insert(sol);
    RelocateSddp relocate(&method);
    ExchangeSddp ex(&method);
    relocate.Optimize(sol);
    ex.Optimize(sol);

    RelatednessSddp relatedness(prob);
    RemoveRandomSddp<Node, Driver> random_remove;
    RemoveRelatedQuickSddp<Node, Driver> related_remove(&relatedness);

    //Relocate<Node, Driver, MoveSddp> relocate(&method);
    //RelocateIntraRoute<Node, Driver, MoveSddp> relocate_intra_route(&method);
    //RelocateIntraRouteV1<Node, Driver, MoveSddp> relocate_intra_route_v1(&method);

    ALNS<Node, Driver> alns;
    alns.AddInsertOperator(&seq);
    //alns.AddInsertOperator(&regret2);
    //alns.AddInsertOperator(&regret2n);
    alns.AddInsertOperator(&regretk);
    alns.AddInsertOperator(&regretkn);
    //alns.AddLocalSearchAlgorithm(&relocate_intra_route);
    //alns.AddLocalSearchAlgorithm(&relocate_intra_route_v1);

    alns.AddRemoveOperator(&random_remove);
    alns.AddRemoveOperator(&related_remove);

    //sol.Show();
    alns.show_output = false;
    alns.SetTemperatureIterInit(0);
    alns.SetTemperature(0.8);
    alns.SetIterationCount(alns_iteration_count);
    alns.Optimize(sol);

    relocate.Optimize(sol);
    ex.Optimize(sol);
    //sol.Show();
    //exit(1);

    if (Parameters::MoveLatestPickupFirst())
        MoveLatestPickupFirst();

    cost = Parameters::MinimizeWaiting() ? cost_func.FinalUpdate(sol) : sol.Update();
    //cost = sol.GetTotalDistances();// + sol.GetUnassignedCount()*100000;
    //nb_unassigneds = sol.GetUnassignedCount();
    distances = sol.GetTotalDistances();

    nb_unassigneds = 0;
    nb_real_unassigneds = 0;
    int k = 0;
    for (int i = 0; i < sol.GetUnassignedCount(); i++) {
        Node *n = sol.GetUnassigned(i);
        if (n->type == NODE_TYPE_PICKUP) {
            k++;
            nb_unassigneds++;
        }
        if (n->type == NODE_TYPE_PICKUP && n->is_real)
            nb_real_unassigneds++;
        if (n->type == NODE_TYPE_PICKUP && n->is_real && n->is_assigned_to >= 0)
            k += 10;
    }
    //cost += k*100000;

}

void Solver::Check() {
    if (sol.GetUsedDriverCount() != sol.GetDriverCount() && sol.GetUnassignedCount() >= 1) {
        printf("Some nodes couldn't be inserted\n");
        sol.Show();

        for (int i = 0; i < sol.GetUnassignedCount(); i++) {
            Node *n = sol.GetUnassigned(i);
            if (n->type != NODE_TYPE_PICKUP)continue;

            for (int j = 0; j < sol.GetDriverCount(); j++)
                if (sol.GetRouteLength(j) == 0) {
                    sol.Show(sol.GetDriver(j));
                    Move<Node, Driver, MoveSddp> m;
                    method.show_output = true;
                    method.InsertCost(sol, n, sol.GetDriver(j), m);
                }
        }

    }
}

void Solver::GetDecisions(Decisions &decs) {
    decs.Clear();
    decs.cost = cost;
    decs.distances = distances;
    decs.nb_unassigneds = nb_unassigneds;
    decs.nb_real_unassigneds = nb_real_unassigneds;

    for (int i = 0; i < prob.GetDriverCount(); i++) {
        Driver *d = prob.GetDriver(i);

        for (Node *n = prob.GetNode(d->StartNodeID); n->type != NODE_TYPE_END_DEPOT; n = sol.GetNext(n)) {
            if (n->type == NODE_TYPE_START_DEPOT) {
                Node *n1 = sol.GetNext(n);
                if (n1->type == NODE_TYPE_END_DEPOT) continue;
            }
            //if(!n->is_real) continue;

            Decision dec;
            dec.is_real = n->is_real;
            dec.decision_type = DECISION_TYPE_ROUTING;
            dec.node_id = n->id;
            dec.node_type = n->type;
            dec.release_time = n->release_time;
            dec.driver_id = d == NULL ? -1 : d->id;
            if (n->IsCustomer()) {
                dec.prev_node_id = d == NULL ? -1 : sol.GetPrev(n)->id;
                dec.prev_node_type = d == NULL ? -1 : sol.GetPrev(n)->type;
                dec.prev_is_real = d == NULL ? -1 : sol.GetPrev(n)->is_real;
                dec.friend_id = n->type == NODE_TYPE_PICKUP ? n->del_node_id : n->pick_node_id;
            }

            dec.next_is_real = d == NULL ? -1 : sol.GetNext(n)->is_real;
            dec.next_node_type = d == NULL ? -1 : sol.GetNext(n)->type;
            dec.time_to_next = d == NULL ? -1 : sol.GetProb()->GetTime(n, sol.GetNext(n));
            dec.next_arrival_time = d == NULL ? -1 : sol.GetNext(n)->arrival_time;
            dec.req_id = n->req_id;
            dec.is_real = n->is_real;
            dec.arrival_time = d == NULL ? -1 : n->arrival_time;
            dec.prev_departure_time = n->prev_departure_time;
            dec.departure_time = n->departure_time;
            if (n->type == NODE_TYPE_DROP && sol.Next[n->id]->type != NODE_TYPE_DROP)
                dec.is_going_to_depot = true;


            decs.Add(dec);
        }
    }

    for (int i = 0; i < sol.GetUnassignedCount(); i++) {
        Node *n = sol.GetUnassigned(i);
        if (!n->is_real) continue;

        Decision dec;
        dec.is_real = n->is_real;
        dec.decision_type = DECISION_TYPE_ROUTING;
        dec.node_id = n->id;
        dec.node_type = n->type;
        dec.driver_id = -1;
        dec.req_id = n->req_id;
        dec.is_real = n->is_real;
        dec.release_time = n->release_time;
        dec.friend_id = n->type == NODE_TYPE_PICKUP ? n->del_node_id : n->pick_node_id;

        if (n->type == NODE_TYPE_PICKUP) {
            Node *drop = sol.GetNode(n->del_node_id);
            dec.is_still_feasible =
                    n->serv_time + Parameters::GetCurrentTime() + prob.GetTime(0, drop->distID) <= drop->tw_end &&
                    n->serv_time + Parameters::GetCurrentTime() + prob.GetTime(0, drop->distID) + drop->serv_time +
                    prob.GetTime(drop->distID, 0) <= Parameters::GetTimeHorizon();
        } else if (n->type == NODE_TYPE_DROP) {
            dec.is_still_feasible = Parameters::GetCurrentTime() + prob.GetTime(0, n->distID) <= n->tw_end &&
                                    Parameters::GetCurrentTime() + prob.GetTime(0, n->distID) + n->serv_time +
                                    prob.GetTime(n->distID, 0) <= Parameters::GetTimeHorizon();
            //dec.is_still_feasible = Parameters::GetCurrentTime() + prob.GetTime(0, n->distID) <= n->tw_end;
        }
        decs.Add(dec);
    }
}


void Solver::SetDecisions(Decisions &decs) {
    sol.PutAllNodesToUnassigned();
    //decs.Sort();

    for (int i = 0; i < sol.GetCustomerCount(); i++) {
        Node *n = sol.GetCustomer(i);
        n->is_fixed_route = false;
        n->is_fixed_pos = false;
        n->is_going_to_depot = false;
        n->is_assigned_to = ~0;
        n->action_type = DECISION_ACTION_NOT_DEFINED;
    }


    for (int i = 0; i < decs.GetCount(); i++) {
        Decision *dec = decs.Get(i);

        if (dec->decision_type == DECISION_TYPE_ASSIGNMENT) {
            Request<Node> *r = sol.GetRequest(dec->req_id);
            if (dec->driver_id > (int) (sizeof(r->GetNode(0)->is_assigned_to) * 8)) {
                printf("There are too many drivers to support DECISION_TYPE_ASSIGNMENT\n");
                printf("Try to increase the number of bits in variables Node::is_assigned_to\n");
                printf("Driver id:%d Capacity:%d\n", dec->driver_id, (int) sizeof(r->GetNode(0)->is_assigned_to) * 8);
                exit(1);
            }
            if (dec->driver_id == -1) {
                r->GetNode(0)->is_assigned_to = 0;
                r->GetNode(1)->is_assigned_to = 0;
            } else if (dec->action_type == DECISION_ACTION_ASSIGN_TO) {
                r->GetNode(0)->is_assigned_to = (1 << dec->driver_id);
                r->GetNode(1)->is_assigned_to = (1 << dec->driver_id);
                if (r->GetNode(0)->action_type == DECISION_ACTION_NOT_DEFINED) {
                    r->GetNode(0)->action_type = DECISION_ACTION_DONT_UNASSIGN;
                    r->GetNode(1)->action_type = DECISION_ACTION_DONT_UNASSIGN;
                }
            } else if (dec->action_type == DECISION_ACTION_DONT_ASSIGN_TO) {
                r->GetNode(0)->is_assigned_to &= ~(1 << dec->driver_id);
                r->GetNode(1)->is_assigned_to &= ~(1 << dec->driver_id);
                if (r->GetNode(0)->action_type == DECISION_ACTION_NOT_DEFINED) {
                    r->GetNode(0)->action_type = DECISION_ACTION_DONT_UNASSIGN;
                    r->GetNode(1)->action_type = DECISION_ACTION_DONT_UNASSIGN;
                }
            }

            /*if(dec->req_id == 1)
            {
            if(dec->action_type == DECISION_ACTION_ASSIGN_TO)
                printf("Req:%d is assigned to:%d action:%d v:", dec->req_id, dec->driver_id, dec->action_type);
            else if(dec->action_type == DECISION_ACTION_DONT_ASSIGN_TO)
                printf("Req:%d is not assigned to:%d action:%d v:", dec->req_id, dec->driver_id, dec->action_type);
            unsigned int m = 1 << (sizeof(r->GetNode(0)->is_assigned_to)*8-1);
            for(int j=0;j<sizeof(r->GetNode(0)->is_assigned_to)*8;j++)
            {
                unsigned int b = (r->GetNode(0)->is_assigned_to & m) == 0?0:1;
                printf("%u", b);
                m =  m >> 1;
            }
            printf("\n");
        }*/
            continue;
        } else if (dec->decision_type == DECISION_TYPE_ACTION) {
            Request<Node> *r = sol.GetRequest(dec->req_id);
            if (r == NULL || r->GetNodeCount() != 2 || dec->req_id >= sol.GetRequestCount() ||
                r->id >= sol.GetRequestCount()) {
                printf("The request in the decision wasn't found in the problem definition\n");
                //r->Show();
                dec->Show();
                printf("\n\n");
                decs.Show();
                prob.Show();

                Parameters::SetDebug(true);
                return;
            }
            if (r->GetNode(0)->action_type == DECISION_ACTION_NOT_DEFINED ||
                r->GetNode(0)->action_type == DECISION_ACTION_DONT_UNASSIGN) {
                //printf("Req:%d prev_action_type:%d new:%d\n", dec->req_id, r->GetNode(0)->action_type, dec->action_type);
                r->GetNode(0)->action_type = dec->action_type;
                r->GetNode(1)->action_type = dec->action_type;
            }
            continue;
        }


        if (dec->driver_id == -1) continue;

        Node *n1 = sol.GetNode(dec->node_id);
        Node *n2 = NULL;

        if (n1->IsCustomer()) {
            sol.RemoveFromUnassigneds(n1);

            if (dec->prev_node_type == NODE_TYPE_START_DEPOT)
                n2 = sol.GetNode(sol.GetDriver(dec->driver_id)->StartNodeID);
            else if (dec->prev_is_real)
                n2 = sol.GetNode(dec->prev_node_id);

            if (n2 == NULL || !sol.IsAssigned(n2)) {
                //printf("Node:%d has a unassigned prev:%d time:%.2lf\n",n1->id, dec->prev_node_id, Paramters::GetTime());
                //dec->Show();

                n2 = sol.GetPrev(sol.GetNode(sol.GetDriver(dec->driver_id)->EndNodeID));
                //printf("inserting after:%d\n", n2->id);
                //decs.Show();
                //exit(1);
                //getchar();
            }

            sol.InsertAfter(n1, n2);
        }

        n1->is_fixed_route = true;
        n1->is_fixed_pos =
                dec->prev_departure_time <= Parameters::GetCurrentTime() || !Parameters::AllowEnRouteDepotReturns();
        n1->arrival_time = dec->arrival_time;
        n1->departure_time = dec->departure_time;
        n1->prev_departure_time = dec->prev_departure_time;
        n1->is_going_to_depot = dec->is_going_to_depot;


        if (n1->type == NODE_TYPE_PICKUP)
            sol.GetNode(n1->del_node_id)->is_fixed_route = true;
    }

    for (int i = 0; i < sol.GetDriverCount(); i++) {
        Driver *d = sol.GetDriver(i);
        d->last_done_node_id = d->StartNodeID;
        Node *prev = sol.GetNode(d->StartNodeID);
        while (prev->type != NODE_TYPE_END_DEPOT) {
            if (prev->is_fixed_pos)
                sol.GetDriver(i)->last_done_node_id = prev->id;
            else if (prev->IsCustomer())
                break;

            prev = sol.Next[prev->id];
        }
    }
    //

    /*if(Parameters::GetCurrentTime() >= 347.71428571428578 - 0.01 && Parameters::GetCurrentTime() <= 347.71428571428578 + 0.01)
    {
        decs.Show();
        for(int i=0;i<sol.GetDriverCount();i++)
        {
            Driver * d = sol.GetDriver(i);
            printf("Driver:%d LastDone:%d arr:%.1lf\n", i, d->last_done_node_id, sol.GetNode(d->last_done_node_id)->arrival_time);
            for(Node * n = sol.GetNode(d->StartNodeID);n->type != NODE_TYPE_END_DEPOT;n = sol.GetNext(n))
                n->Show();
        }
        sol.Show();
    }*/
}

void Solver::Unfix() {
    for (int i = 0; i < prob.GetNodeCount(); i++) {
        prob.GetNode(i)->is_fixed_pos = false;
        prob.GetNode(i)->is_fixed_route = false;
    }
    for (int i = 0; i < sol.GetDriverCount(); i++) {
        Driver *d = sol.GetDriver(i);
        d->last_done_node_id = d->StartNodeID;
    }
}

void Solver::MoveLatestPickupFirst() {
    for (int i = 0; i < sol.GetDriverCount(); i++) {
        Driver *d = sol.GetDriver(i);
        //d->last_done_node_id = d->StartNodeID;
        Node *prev = sol.GetNode(d->last_done_node_id == -1 ? d->StartNodeID : d->last_done_node_id);
        Node *latest_pick = NULL;
        Node *latest_wait_pick = NULL;
        Node *first_pick = NULL;

        while (prev->type != NODE_TYPE_END_DEPOT) {
            Node *next = sol.GetNext(prev);
            if (next->type == NODE_TYPE_PICKUP) {
                if (first_pick == NULL)
                    first_pick = next;
                if (latest_pick == NULL)
                    latest_pick = next;
                else if (latest_pick->release_time < next->release_time)
                    latest_pick = next;

            }
                //we reached a drop, time to move the latest pickup first
            else {
                if (first_pick != NULL && first_pick != latest_pick) {
                    //printf("Moving %d before %d next:%d\n",latest_pick->id, first_pick->id,next->id);
                    //sol.Show(d);

                    sol.Remove(latest_pick);
                    sol.InsertBefore(latest_pick, first_pick);
                    //sol.Update(d);
                }


                latest_pick = first_pick = NULL;
            }

            prev = next;
        }
    }

}

void Solver::Update() {
    cost = cost_func.FinalUpdate(sol);//sol.Update();
    nb_unassigneds = sol.GetUnassignedCount();
    distances = sol.GetTotalDistances();
}

int Solver::GetRouteCount() {
    int nb = 0;
    for (int i = 0; i < sol.GetDriverCount(); i++)
        for (Node *n = sol.GetNode(sol.GetDriver(i)->StartNodeID); n->type != NODE_TYPE_END_DEPOT;) {
            Node *next = sol.GetNext(n);
            if (n->type == NODE_TYPE_DROP && (next->type == NODE_TYPE_PICKUP || next->type == NODE_TYPE_END_DEPOT))
                nb++;
            n = next;
        }
    return nb;
}


int Solver::EvaluateAllWait() {
    int init_waiting_time = Parameters::GetWaitingTime();

    double init_cost = sol.GetCost();

    std::vector<int> actions(sol.GetNodeCount(), DECISION_ACTION_NOT_DEFINED);
    for (int i = 0; i < sol.GetDriverCount(); i++)
        for (Node *n = sol.GetNode(sol.GetDriver(i)->last_done_node_id);
             n->type != NODE_TYPE_END_DEPOT; n = sol.GetNext(n))
            if (n->IsCustomer()) {
                actions[n->id] = n->action_type;

                if (n->arrival_time >= Parameters::GetCurrentTime())
                    n->action_type = DECISION_ACTION_WAIT;
            }

    cost = sol.GetCost();
    //printf("Cost:%lf\n", cost);
    if (cost >= BIG_DOUBLE) {
        //sol.Show();
        for (int i = 0; i < sol.GetDriverCount(); i++)
            cost_func.MakeFeasible(sol, sol.GetDriver(i));

        Optimize();
        //printf("Cost:%lf\n", cost);
    }


    int waiting = init_waiting_time + 1;
    while (true && waiting - init_waiting_time <= 25) {
        Parameters::SetWaitingTime(waiting);
        cost = sol.GetCost();
        if (cost < BIG_DOUBLE)
            waiting++;
        else {
            waiting--;
            Parameters::SetWaitingTime(waiting);
            break;
        }
    }
    //printf("PrevAllowed waiting:%d\n", Parameters::GetWaitingTime() );
    cost = Parameters::MinimizeWaiting() ? cost_func.FinalUpdate(sol) : sol.Update();

    for (int i = 0; i < sol.GetDriverCount(); i++)
        for (Node *n = sol.GetNode(sol.GetDriver(i)->last_done_node_id);
             n->type != NODE_TYPE_END_DEPOT; n = sol.GetNext(n))
            if (n->IsCustomer())
                n->action_type = actions[n->id];
    //printf("Allowed waiting:%d\n", waiting);
    Parameters::SetWaitingTime(init_waiting_time);
    return waiting;
}

int Solver::GetFutureCustomerAssignmentCount() {
    int nb = 0;
    for (int i = 0; i < sol.GetDriverCount(); i++)
        for (Node *n = sol.GetNode(sol.GetDriver(i)->last_done_node_id);
             n->type != NODE_TYPE_END_DEPOT; n = sol.GetNext(n))
            if (n->type == NODE_TYPE_PICKUP && n->is_real && n->release_time >= 0.1)
                nb++;
    return nb;
}

int Solver::GetFutureCustomerCount() {
    int nb = 0;
    for (int i = 0; i < sol.GetCustomerCount(); i++)
        if (sol.GetCustomer(i)->type == NODE_TYPE_PICKUP && sol.GetCustomer(i)->is_real &&
            sol.GetCustomer(i)->release_time >= 0.1)
            nb++;
    return nb;
}

int Solver::GetInitialCustomerUnassignmentCount() {
    int nb = 0;
    for (int i = 0; i < sol.GetUnassignedCount(); i++) {
        Node *n = sol.GetUnassigned(i);
        if (n->type == NODE_TYPE_PICKUP && n->is_real && n->release_time <= 0.1)
            nb++;
    }
    return nb;
}

void Solver::GetSolutionCompact(SolCompact<Node, Driver> &s) {
    s.unassigneds.clear();
    s.routes.clear();
    s.cost = sol.GetCost();
    s.distance = sol.GetTotalDistances();
    for (int i = 0; i < sol.GetUnassignedCount(); i++)
        s.unassigneds.push_back(*sol.GetUnassigned(i));

    s.routes.resize(sol.GetDriverCount());
    for (int i = 0; i < sol.GetDriverCount(); i++)
        for (Node *n = sol.GetNode(sol.GetDriver(i)->StartNodeID); n != NULL; n = sol.GetNext(n))
            s.routes[i].push_back(*n);
}

void Solver::GetReport(Report &report) {
    report.Build(sol);
}
