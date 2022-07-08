//Not considering Capacity and Time windows constraints.

#include "InsRmvMethodSddp.h"
#include "../../../lib/mathfunc.h"
#include "../constants.h"
#include <algorithm>
#include "Decision.h"
#include "Parameters.h"

InsRmvMethodSddp::InsRmvMethodSddp(Prob<Node,Driver> * prob, CostFunctionSddp & cost_func): p(prob), _cost_func(cost_func)
{
	_added_noise = mat_func_get_max_smaller_than(p->GetDistances(), p->GetDimension(), INFINITE);
	_added_noise *= 0.025;
	SetNoise(0);
	show_output = false;
}

InsRmvMethodSddp::~InsRmvMethodSddp(){}

void InsRmvMethodSddp::InsertCost(Sol<Node,Driver> & s, Node * pick, Driver * d, Move<Node,Driver,MoveSddp> & mo)
{
	mo.IsFeasible = false;
	mo.DeltaCost = INFINITE;
	mo.n = pick;
	mo.to = d;
	Node* drop = s.GetNode(pick->del_node_id);//s.GetProb()->GetRequest(pick)->GetDropNode(0);



	if(pick->is_fixed_pos)
	{
		printf("Node:%d is fixed. It shouldn't be inserted\n", pick->id);
		pick->Show();
		s.Show();
		Node * nnn = NULL;
		nnn->id = 4;
		exit(1);
	}

	//if(pick->is_assigned_to != -2 && pick->is_assigned_to != d->id) return;
	if( (pick->is_assigned_to & (1 << d->id)) == 0) return;

	/*if(show_output)
	{
		printf("InsertCost for request:%d driver:%d\n", pick->req_id, d->id);
		pick->Show();
		drop->Show();
		printf("Time to drop:%.2lf\n", s.GetProb()->GetTime(s.GetNode(d->StartNodeID), drop));
	}*/

	Node * prev_pick = s.GetNode( d->last_done_node_id==-1?d->StartNodeID:d->last_done_node_id );
	while (prev_pick->type != NODE_TYPE_END_DEPOT)
	{
		Node * next_pick = s.Next[prev_pick->id];

		//dont insert the pickup between other pickups, skip these moves
		if(prev_pick->type == NODE_TYPE_PICKUP && next_pick->type == NODE_TYPE_PICKUP)
		{
			prev_pick = next_pick;
			continue;
		}
		/*if(next_pick->type == NODE_TYPE_PICKUP && pick->release_time < next_pick->release_time)
		{
			prev_pick = next_pick;
			continue;
		}*/
		/*if(prev_pick->type == NODE_TYPE_PICKUP && next_pick->type == NODE_TYPE_PICKUP)
		{
			prev_pick = next_pick;
			continue;
		}*/

		double pick_arr = prev_pick->departure_time + p->GetTime(prev_pick,pick);
		if(pick->release_time  > pick_arr)
			pick_arr = pick->release_time ;

		//if(show_output) printf("prev:%d pick_arr:%.2lf\n", prev_pick->id, pick_arr);

		//if the arrival time at the node isn't feasible
		//All the next positions will be infeasible
		if (pick_arr > pick->tw_end+0.0001) break;

		if(pick->action_type == DECISION_ACTION_WAIT && (prev_pick->action_type == DECISION_ACTION_GO_NOW || next_pick->action_type == DECISION_ACTION_GO_NOW))
		{
			prev_pick = next_pick;
			continue;
		}
		if(pick->action_type == DECISION_ACTION_GO_NOW &&
				(next_pick->action_type == DECISION_ACTION_WAIT || next_pick->release_time > Parameters::GetCurrentTime() || pick_arr > Parameters::GetCurrentTime()))
		{
			prev_pick = next_pick;
			continue;
		}
		if((prev_pick->action_type == DECISION_ACTION_GO_NOW || next_pick->action_type == DECISION_ACTION_GO_NOW) && pick->release_time > Parameters::GetCurrentTime())
		{
			prev_pick = next_pick;
			continue;
		}

		double next_pick_arr = std::max(pick_arr, pick->tw_start) + pick->serv_time;
		if(next_pick_arr < next_pick->release_time && next_pick->type == NODE_TYPE_DROP)
			next_pick_arr = next_pick->release_time ;
    next_pick_arr += p->GetTime( pick , next_pick );
		if(next_pick_arr < next_pick->release_time)
			next_pick_arr = next_pick->release_time ;

		//if(show_output) printf("\tnext_pick_arr:%.2lf\n", next_pick_arr);
        //if the arrival time at the next node is after its upper tw
        //no need to call s.GetCost(d)
        //Just go to the next node
		if(next_pick_arr > next_pick->tw_end+0.001)
		{
			prev_pick = next_pick;
			continue;
		}

		s.InsertAfter(pick, prev_pick);
		Node * prev_drop = pick;
		double prev_drop_arr = pick_arr;
		while (prev_drop->type != NODE_TYPE_END_DEPOT)
		{
			Node * next_drop = s.Next[prev_drop->id];

			if(prev_drop->type == NODE_TYPE_PICKUP && prev_drop != pick)
				break;

			double drop_arr = std::max(prev_drop_arr, prev_drop->tw_start) + prev_drop->serv_time;
			if(drop_arr < drop->release_time )
				drop_arr = drop->release_time ;
			drop_arr += p->GetTime(prev_drop,drop);

			//if(show_output) printf("\t\tdrop_arr:%.2lf\n", drop_arr);
			//if the arrival time at the node isn't feasible
			//All the next positions will be infeasible
			if(drop_arr > drop->tw_end+0.001)
			{
				if(prev_drop == pick)
				{
					s.Remove(pick);
					return;
				}
				else
					break;
			}


			double next_drop_arr = std::max(drop_arr, drop->tw_start) + drop->serv_time;
			if(next_drop->type == NODE_TYPE_PICKUP)
			{
				next_drop_arr += p->GetTime(drop,next_drop);
				if(next_drop->release_time  > next_drop_arr)
					next_drop_arr = next_drop->release_time ;
			}
			else if(next_drop->type == NODE_TYPE_DROP)
			{
				if(next_drop->release_time  > next_drop_arr)
					next_drop_arr = next_drop->release_time ;
				next_drop_arr += p->GetTime(drop,next_drop);
			}
			else
				next_drop_arr += p->GetTime(drop,next_drop);


			double new_prev_drop_arr = std::max(prev_drop_arr, prev_drop->tw_start) + prev_drop->serv_time;
			if(next_drop->type == NODE_TYPE_PICKUP)
			{
				new_prev_drop_arr += p->GetTime(prev_drop,next_drop);
				if(next_drop->release_time  > new_prev_drop_arr)
					new_prev_drop_arr = next_drop->release_time ;
			}
			else if(next_drop->type == NODE_TYPE_DROP)
			{
				if(next_drop->release_time  > new_prev_drop_arr)
					new_prev_drop_arr = next_drop->release_time ;
				new_prev_drop_arr += p->GetTime(prev_drop,next_drop);
			}

			//if the arrival time at the next node is after its upper tw
	    //no need to call s.GetCost(d)
	    //Just go to the next node
			if(next_drop_arr > next_drop->tw_end+0.001)
			{
				prev_drop_arr = new_prev_drop_arr;
				prev_drop = next_drop;
				continue;
			}

			/*if(prev_drop->type == NODE_TYPE_PICKUP && prev_drop != pick && next_drop->type == NODE_TYPE_PICKUP)
			{
				prev_drop_arr = new_prev_drop_arr;
				prev_drop = next_drop;
				continue;
			}*/

			double deltaDist = 0;
			if(prev_drop == pick)
				deltaDist = p->GetDist(prev_pick,pick) +
										p->GetDist(pick,drop) +
							   		p->GetDist(drop,next_drop) -
							   		p->GetDist(prev_pick,next_drop);
			else
				deltaDist = p->GetDist(prev_pick,pick) +
								p->GetDist(pick,next_pick) -
								p->GetDist(prev_pick,next_pick) +
								p->GetDist(prev_drop,drop) +
								p->GetDist(drop,next_drop) -
								p->GetDist(prev_drop,next_drop);

			double newcost = deltaDist + GetNoise() * (2 * mat_func_get_rand_double() * _added_noise - _added_noise);
			if(newcost >= mo.DeltaCost)
			{
				prev_drop_arr = new_prev_drop_arr;
				prev_drop = next_drop;
				continue;
			}

			s.InsertAfter(drop, prev_drop);
			double cost = _cost_func.GetCost(s,d);
			s.Remove(drop);
			if(cost < BIG_DOUBLE)
			{
				mo.DeltaCost = newcost;
				mo.IsFeasible = true;
				mo.move.prev_pick = prev_pick;
				mo.move.prev_drop = prev_drop;
			}

			prev_drop_arr = new_prev_drop_arr;
			prev_drop = next_drop;
		}
		s.Remove(pick);
		prev_pick = next_pick;
	}
}

void InsRmvMethodSddp::ApplyInsertMove(Sol<Node,Driver> & s, Move<Node,Driver,MoveSddp> & m)
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



	//if the pickup is inserted among pickups, put it in the sorted order of release time
	Node * pp = m.move.prev_pick;
	if(s.GetNext(pp)->type == NODE_TYPE_PICKUP)
		while(pp->id != m.to->last_done_node_id && pp->type == NODE_TYPE_PICKUP && pp->release_time > m.n->release_time)
			pp = s.GetPrev(pp);
	s.InsertAfter(m.n, pp);

	if(m.n == m.move.prev_drop && pp != m.move.prev_pick)
		s.InsertAfter(drop, m.move.prev_pick);
	else
		s.InsertAfter(drop, m.move.prev_drop);

	//s.InsertAfter(m.n, m.move.prev_pick);
	//s.InsertAfter(drop, m.move.prev_drop);
}

void InsRmvMethodSddp::RemoveCost(Sol<Node,Driver> & s, Node * pick, Move<Node,Driver,MoveSddp> & m)
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
