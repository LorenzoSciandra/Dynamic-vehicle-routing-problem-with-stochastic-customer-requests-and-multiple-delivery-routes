//Not considering Capacity and Time windows constraints.

#include "InsRmvMethodTsp.h"
#include "../../../lib/mathfunc.h"
#include "../constants.h"
#include <algorithm>
#include "Decision.h"
#include "Parameters.h"

InsRmvMethodTsp::InsRmvMethodTsp(Prob<Node,Driver> * prob): p(prob)
{
	_added_noise = mat_func_get_max_smaller_than(p->GetDistances(), p->GetDimension(), INFINITE);
	_added_noise *= 0.025;
	SetNoise(0);
	show_output = false;
}

InsRmvMethodTsp::~InsRmvMethodTsp(){}

void InsRmvMethodTsp::InsertCost(Sol<Node,Driver> & s, Node * pick, Driver * d, Move<Node,Driver,MoveSddp> & mo)
{
	mo.IsFeasible = false;
	mo.DeltaCost = INFINITE;
	mo.n = pick;
	mo.to = d;
	Node* drop = s.GetNode(pick->del_node_id);//s.GetProb()->GetRequest(pick)->GetDropNode(0);//Get correspondant delivery node from request

	if(pick->is_assigned_to != -2 && pick->is_assigned_to != d->id) return;

	double max_duration = s.GetNode(d->EndNodeID)->tw_end - s.GetNode(d->StartNodeID)->tw_start;
	Node * start_depot = s.GetNode(d->StartNodeID);

	Node * prev = s.GetNode( d->last_done_node_id==-1?d->StartNodeID:d->last_done_node_id );
	while (prev->type != NODE_TYPE_END_DEPOT)
	{
		Node * next = s.Next[prev->id];
		if(prev->type == NODE_TYPE_PICKUP)
		{
			prev = next;
			continue;
		}

		double deltatime = 0;
		if(prev == start_depot)
			deltatime = p->GetTime(prev,pick) +
							p->GetTime(pick,drop) +
							p->GetTime(drop,next) -
							p->GetTime(prev,next) + 5;
		else
			deltatime = p->GetTime(prev,drop) +
							p->GetTime(drop,next) -
							p->GetTime(prev,next);

		double deltaDist = p->GetDist(prev,drop) +
								 p->GetDist(drop,next) -
								 p->GetDist(prev,next);
		double newcost = deltaDist + GetNoise() * (2 * mat_func_get_rand_double() * _added_noise - _added_noise);


		if(newcost < mo.DeltaCost && deltatime <= max_duration)
		{
			mo.DeltaCost = newcost;
			mo.IsFeasible = true;
			mo.move.prev_pick = s.GetNode(d->StartNodeID);
			mo.move.prev_drop = next;
		}

		prev = next;
	}
}

void InsRmvMethodTsp::ApplyInsertMove(Sol<Node,Driver> & s, Move<Node,Driver,MoveSddp> & m)
{
	Node* drop = s.GetProb()->GetRequest(m.n)->GetNode(1);
	if(m.from != NULL)
	{
		s.Remove(m.n); //Remove Pickup node
		s.Remove(drop); //Remove correspondant delivery node
		m.from->sum_demand -=  m.n->demand + drop->demand;
	}
	else
	{
		s.RemoveFromUnassigneds(m.n);
		s.RemoveFromUnassigneds(drop);
	}
	s.InsertAfter(m.n, m.move.prev_pick);
	s.InsertAfter(drop, m.move.prev_drop);
}

void InsRmvMethodTsp::RemoveCost(Sol<Node,Driver> & s, Node * pick, Move<Node,Driver,MoveSddp> & m)
{
	Node * drop = s.GetNode(pick->del_node_id);
	m.IsFeasible = true;
	m.n = pick;
	m.to = NULL;
	m.move.prev_pick = NULL;
	m.move.prev_drop = NULL;
	m.from = s.GetAssignedTo(pick);
	double deltaDist = 0;
	if(m.from != NULL)
	{
		m.move.prev_pick = s.GetPrev(pick);
		m.move.prev_drop = s.GetPrev(drop);
		if(m.move.prev_drop == pick)
			deltaDist = - p->GetDist(m.move.prev_pick,pick) -
							p->GetDist(pick,drop) -
							p->GetDist(drop, s.GetNext(drop) ) +
							p->GetDist(m.move.prev_pick, s.GetNext(drop));
		else
			deltaDist = - p->GetDist(m.move.prev_pick,pick) -
							p->GetDist(pick, s.GetNext(pick) ) -
							p->GetDist(m.move.prev_drop,drop) -
							p->GetDist(drop, s.GetNext(drop)) +
							p->GetDist(m.move.prev_pick, s.GetNext(pick))+
							p->GetDist(m.move.prev_drop, s.GetNext(drop));

	}

	m.DeltaCost = m.DeltaDistance = deltaDist;
}
