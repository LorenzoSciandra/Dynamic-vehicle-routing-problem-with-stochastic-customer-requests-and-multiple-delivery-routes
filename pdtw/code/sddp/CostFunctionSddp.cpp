

#include "CostFunctionSddp.h"
#include "../constants.h"
#include "../Request.h"
#include "stdio.h"
#include "Parameters.h"
#include "Decision.h"

double CostFunctionSddp::GetCost(Sol<Node,Driver> & s)
{
	double cost = 0;
	for(int i = 0 ; i < s.GetDriverCount() ; i++)
		cost += GetCost(s, s.GetDriver(i));

	int k = 0;
	//int k = s.GetUnassignedCount();
  for (int i =0 ; i < s.GetUnassignedCount() ; i++)
	{
		Node * n = s.GetUnassigned(i);
		if(n->type == NODE_TYPE_PICKUP)
		{
			k++;
			if(n->is_real) k+= 100;
			if(n->action_type == DECISION_ACTION_GO_NOW || n->action_type == DECISION_ACTION_WAIT || n->action_type == DECISION_ACTION_DONT_UNASSIGN)
				k+= 100;
		}
	}

	return cost + k*100000;
}

double CostFunctionSddp::GetCost(Sol<Node,Driver> & s, Driver * d)
{
	//Calculate the cost of the route d
	//If the time window constraint is violated, the function returns BIG_DOUBLE
	double cost=0;
	Node * prev = s.GetNode( d->last_done_node_id==-1?d->StartNodeID:d->last_done_node_id );
	double time = prev->tw_start;   //earliest departure from the start depot
	if(prev->is_fixed_pos)
		time = prev->departure_time;

	//if we were waiting at the depot
	if(time < Parameters::GetCurrentTime() && !prev->is_going_to_depot)
	   time = Parameters::GetCurrentTime();

	//if our last node is a drop and we arrived at the depot before the current time
	if(time + p->GetTime(prev->distID,0) < Parameters::GetCurrentTime() && prev->is_going_to_depot)
		time = Parameters::GetCurrentTime()-p->GetTime(prev->distID,0);

	int go_now_count = 0;
	while(prev->type != NODE_TYPE_END_DEPOT)
	{
		Node * next = s.GetNext(prev);

		if(Parameters::ForbidStochasticDropAfterReal() && next->type == NODE_TYPE_DROP && !next->is_real && prev->is_real)
			return BIG_DOUBLE;

		if(time < next->release_time && next->type == NODE_TYPE_DROP)
			time = next->release_time;

		if(prev->is_going_to_depot && prev->type == NODE_TYPE_DROP && next->type == NODE_TYPE_DROP)
		{
			time += p->GetTime(prev->distID,0) + p->GetTime(0,next->distID);//0 is the depot
			cost += p->GetDist(prev->distID,0) + p->GetDist(0,next->distID);
		}
		else
		{
			time += p->GetTime(prev,next);
			cost += p->GetDist(prev,next);
		}

		//add one minute if we wait to deliver the request
		if(next->type == NODE_TYPE_PICKUP && next->action_type == DECISION_ACTION_WAIT && time <= Parameters::GetCurrentTime()+Parameters::GetWaitingTime())
			time = Parameters::GetCurrentTime() + Parameters::GetWaitingTime();

		if(next->type == NODE_TYPE_PICKUP && next->action_type == DECISION_ACTION_GO_NOW && time > Parameters::GetCurrentTime()+0.01)
		{
			if(Parameters::IsDebug()) printf("infeasible go now node:%d\n", next->id);
			return BIG_DOUBLE;
		}

		if(next->type == NODE_TYPE_PICKUP && next->action_type == DECISION_ACTION_GO_NOW)
			go_now_count++;

		if(next->type == NODE_TYPE_DROP && next->action_type == DECISION_ACTION_GO_NOW)
			go_now_count--;

		if(go_now_count >= 1 && (next->release_time > Parameters::GetCurrentTime() || next->action_type == DECISION_ACTION_WAIT))
		{
			if(Parameters::IsDebug()) printf("infeasible has go now node:%d\n", next->id);
			return BIG_DOUBLE;
		}


		if(time < next->tw_start)
			time = next->tw_start;
		else if(time > next->tw_end+0.0001)
		{
			if(Parameters::IsDebug()) printf("infeasible upper tw node:%d\n", next->id);
			return BIG_DOUBLE;
		}

		time += next->serv_time;
		prev = next;
	}

	if(time > prev->tw_end)
		return BIG_DOUBLE;
	else
		return cost;
}

double CostFunctionSddp::Update(Sol<Node,Driver> & s)
{
	double sumd = 0;
	for(int i = 0 ; i < s.GetDriverCount() ; i++)
	{
		Update(s, s.GetDriver(i));
		sumd += s.GetDriver(i)->cur_distance;
	}
	s.SetTotalDistances(sumd);

	int k = 0;
	//int k = s.GetUnassignedCount();
   for (int i =0 ; i < s.GetUnassignedCount() ; i++)
	{
		Node * n = s.GetUnassigned(i);
		if(n->type == NODE_TYPE_PICKUP)
		{
			k++;
			if(n->is_real) k+= 100;
			if(n->action_type == DECISION_ACTION_GO_NOW || n->action_type == DECISION_ACTION_WAIT || n->action_type == DECISION_ACTION_DONT_UNASSIGN)
				k+= 100;
		}
	}

	return sumd + k*100000;
}

double CostFunctionSddp::Update(Sol<Node,Driver> & s, Driver * d)
{
	d->cur_distance = 0;
	Node * prev = s.GetNode( d->last_done_node_id==-1?d->StartNodeID:d->last_done_node_id );
	double time = prev->tw_start;   //earliest departure from the start depot
	if(prev->is_fixed_pos)
	   time = prev->departure_time;
	else
	{
		prev->arrival_time = time;
		prev->departure_time = time;
	}

	//if we were waiting at the depot
	if(time < Parameters::GetCurrentTime() && !prev->is_going_to_depot)
	   time = Parameters::GetCurrentTime();

	//if the last performed node is a drop and we arrived at the depot before the current time
	if(time + p->GetTime(prev->distID,0) < Parameters::GetCurrentTime() && prev->is_going_to_depot)
		time = Parameters::GetCurrentTime()-p->GetTime(prev->distID,0);

	//printf("Start time:%lf prev:%lf fixed:%d\n", time,prev->arrival_time, (int)prev->is_fixed_pos);
	while(prev->type != NODE_TYPE_END_DEPOT)
	{
		Node * next = s.GetNext(prev);

		if(time < next->release_time && next->type == NODE_TYPE_DROP)
			time = next->release_time;

		if(!prev->is_fixed_pos)
			prev->departure_time = time;

		double tt = 0;
		if(prev->is_going_to_depot && prev->type == NODE_TYPE_DROP && next->type == NODE_TYPE_DROP)
			tt = p->GetTime(prev->distID,0) + p->GetTime(0,next->distID);
		else
			tt = p->GetTime(prev,next);
		time += tt;
		d->cur_duration += tt;


		if(next->type == NODE_TYPE_PICKUP && next->action_type == DECISION_ACTION_WAIT && time <= Parameters::GetCurrentTime()+Parameters::GetWaitingTime())
			time = Parameters::GetCurrentTime() + Parameters::GetWaitingTime();

		next->arrival_time = time;
		next->prev_departure_time = prev->departure_time;

		if(time < next->tw_start)
			 time = next->tw_start;
		else if(time > next->tw_end + 0.0001)
		{
			printf("\n\nWe shouldn't enter here Time:%lf\n", Parameters::GetCurrentTime() );
			for(Node * n = s.GetNode(d->StartNodeID);n != NULL;n = s.GetNext(n))
				n->Show();
			 s.Show(d);

			 printf("if(cur_time > next->tw_end)\n");
			 printf("Next no:%d time:%.10lf next->late_tw:%.10lf\n", next->id, time, next->tw_end);
			 Node * nnn = NULL;
			 nnn->id = 4;
			 exit(1);
		}

		if(next->is_fixed_pos)
		{
			 printf("Next:%d is fixed and the LastNodeID should be higher than this:%d\n",next->id, d->last_done_node_id);

			 for(Node * n = s.GetNode(d->StartNodeID);n->type != NODE_TYPE_END_DEPOT;n = s.GetNext(n))
 				n->Show();
 			 s.Show(d);

			 Node * nnn = NULL;
			 nnn->id = 4;
			 exit(1);
		}
		time += next->serv_time;
		d->cur_duration += next->serv_time;
		d->cur_distance += p->GetDist(prev,next);
		prev = next;
	}

	prev->arrival_time = time;
	return d->cur_distance;
}

double CostFunctionSddp::FinalUpdate(Sol<Node,Driver> & s)
{
	double cost = Update(s);
	for(int i = 0 ; i < s.GetDriverCount() ; i++)
		FinalUpdate(s, s.GetDriver(i));

	//if(Parameters::GetCurrentTime() >= 11.9)
	//	exit(1);

	return cost;
}

//just make sure that arrival time are the latest possible to minimize waiting times
double CostFunctionSddp::FinalUpdate(Sol<Node,Driver> & s, Driver * d)
{

	/*bool has_req1 = false;
	if(Parameters::GetCurrentTime() >= 11.9)
	{

		for(Node * n = s.GetNode(d->StartNodeID);n->type != NODE_TYPE_END_DEPOT;n = s.GetNext(n))
			if(n->req_id == 1 && s.GetNext(n)->req_id == 1)
			{
				has_req1 = true;
				break;
			}

		if(has_req1) printf("\nFinalUpdate Driver:%d\n", d->id);
		if(has_req1)
		for(Node * n = s.GetNode(d->StartNodeID);n->type != NODE_TYPE_END_DEPOT;n = s.GetNext(n))
	  		n->Show();

	}*/
	//printf("\n");

	Node * first_seen_pickup = NULL;
	Node * prev = s.GetNode(d->last_done_node_id); //at this point, last done node id was correctly set
	while(prev->type != NODE_TYPE_END_DEPOT)
	{
		Node * next = s.GetNext(prev);

		if(next->action_type == DECISION_ACTION_GO_NOW) break;

		if(!prev->is_fixed_pos && prev->type == NODE_TYPE_PICKUP && first_seen_pickup == NULL)
			first_seen_pickup = prev;

		//if we have an unfixed pickup (first_seen_pickup != NULL) and we are returning to the depot,
		//then post-pone all arrival times from first_seen_pickup to the drop (prev)
		if(prev->type == NODE_TYPE_DROP && next->type != NODE_TYPE_DROP && first_seen_pickup != NULL)
		{
			double slack = 0;
			Node * n = prev;
			while(true)
			{
				slack = std::max(0.0, n->tw_start - n->arrival_time) +
						  std::min(n->tw_end - std::max(n->tw_start, n->arrival_time), slack);
				if(first_seen_pickup == n) break;
				n = s.GetPrev(n);
			}

			//if(has_req1)
			{
			//	printf("first_seen_pickup:%d prev:%d next:%d n:%d slack:%.2lf\n",first_seen_pickup->id, prev->id, next->id, n->id, slack);
			//	n->Show();
			}


			Node * stop_node = prev;
			prev = first_seen_pickup;
			prev->arrival_time += slack;
			double time = prev->arrival_time;
			while(prev != stop_node)
			{
				Node * next = s.GetNext(prev);
				if(time < next->release_time && next->type == NODE_TYPE_DROP)
					time = next->release_time;

				if(!prev->is_fixed_pos)
					prev->departure_time = time;

				if(prev->is_going_to_depot && prev->type == NODE_TYPE_DROP && next->type == NODE_TYPE_DROP)
					time += p->GetTime(prev->distID,0) + p->GetTime(0,next->distID);
				else
					time += p->GetTime(prev,next);

				if(next->type == NODE_TYPE_PICKUP && next->action_type == DECISION_ACTION_WAIT && time <= Parameters::GetCurrentTime()+Parameters::GetWaitingTime())
					time = Parameters::GetCurrentTime() + Parameters::GetWaitingTime();

				next->arrival_time = time;
				next->prev_departure_time = prev->departure_time;

				if(time < next->tw_start)
					 time = next->tw_start;
				time += next->serv_time;

				prev = next;
			}


			break; //no need to push further the next nodes
		}

		prev = next;
	}

	/*if(Parameters::GetCurrentTime() >= 11.9)
	{

		if(has_req1)
			for(Node * n = s.GetNode(d->StartNodeID);n->type != NODE_TYPE_END_DEPOT;n = s.GetNext(n))
	  			n->Show();
		//if(has_req1)
		//	exit(1);
	}*/
	return d->cur_distance;
}

void CostFunctionSddp::Show(Sol<Node,Driver> * s)
{
	printf("Solution cost:%.4lf\n", GetCost(*s) );
	for(int i = 0; i<s->GetDriverCount(); i++)
		Show(s,s->GetDriver(i));

	printf("Unassigneds(%d):", s->GetUnassignedCount());
	for(int i = 0; i<s->GetUnassignedCount(); i++)
	{
		Node * n = s->GetUnassigned(i);
		if(n->type == NODE_TYPE_DROP)
			printf("%d(rel:%.0lf ut:%.0lf real:%d) ", n->id, n->release_time,n->tw_end, (int)n->is_real);
	}
	printf("\n");
}
void CostFunctionSddp::Show(Sol<Node,Driver> * s, Driver * d)
{
	double cost = GetCost(*s, d);
	printf("Route:%d cost:%.2lf LastDone:%d len:%d nodes:", d->id, cost,d->last_done_node_id, s->GetRouteLength(d));
	Node * cur = s->GetNode(d->StartNodeID);
	while (cur != NULL)
	{
		//if(cur->type != NODE_TYPE_START_DEPOT && cur->type != NODE_TYPE_END_DEPOT)
			//printf("%d(%d)\n", cur->no, _prob->GetRequest(cur)->GetNode(1)->id + 1);
		/*printf("%d", cur->id);
		if(cur->type == NODE_TYPE_DROP)
			printf("d");
		if(cur->type == NODE_TYPE_PICKUP)
			printf("p");
		if(cur->action_type == DECISION_ACTION_WAIT)
			printf("w");
		if(cur->action_type == DECISION_ACTION_GO_NOW)
			printf("g");
		printf(" ");*/
		if(cur->IsCustomer())
		{
			double tt = s->GetProb()->GetTime(cur, s->GetNext(cur));
			//printf("%d(rel:%.0lf ar:%.0lf dep:%.0lf tt:%.1lf)-", cur->id,cur->release_time, cur->arrival_time, cur->departure_time,tt);
			printf("%d", cur->id);
			if(cur->type == NODE_TYPE_PICKUP)
				printf("p");
			else if(cur->type == NODE_TYPE_DROP)
				printf("d");
			printf("(ac:%d rel:%.0lf arr:%.1lf tw:%.0lf-%.0lf)-",cur->action_type, cur->release_time, cur->arrival_time,cur->tw_start, cur->tw_end);
		}
		else if(cur->type == NODE_TYPE_START_DEPOT)
			printf("%d(dep:%.0lf)-", cur->id,cur->departure_time);
		else if(cur->type == NODE_TYPE_END_DEPOT)
			printf("%d(ar:%.0lf)-", cur->id,cur->arrival_time);
		//	printf("0-");
		cur = s->Next[cur->id];
	}
	printf("\n");
}

void CostFunctionSddp::MakeFeasible(Sol<Node,Driver> & s, Driver * d)
{
	Node * prev = s.GetNode( d->last_done_node_id==-1?d->StartNodeID:d->last_done_node_id );
	double time = prev->tw_start;   //earliest departure from the start depot
	if(prev->is_fixed_pos)
	   time = prev->departure_time;

	if(time < Parameters::GetCurrentTime() && !prev->is_going_to_depot)
	   time = Parameters::GetCurrentTime();

	//if the last performed node is a drop and we arrived at the depot before the current time
	if(time + p->GetTime(prev->distID,0) < Parameters::GetCurrentTime() && prev->is_going_to_depot)
		time = Parameters::GetCurrentTime()-p->GetTime(prev->distID,0);

	double start_time = time;
	int nbrestarts = 0;
	while(prev->type != NODE_TYPE_END_DEPOT)
	{
		Node * next = s.GetNext(prev);
		if(time < next->release_time && next->type == NODE_TYPE_DROP)
			time = next->release_time;

		if(prev->is_going_to_depot && prev->type == NODE_TYPE_DROP && next->type == NODE_TYPE_DROP)
			time += p->GetTime(prev->distID,0) + p->GetTime(0,next->distID);
		else
			time += p->GetTime(prev,next);

		if(Parameters::ForbidStochasticDropAfterReal() && next->type == NODE_TYPE_DROP && !next->is_real && prev->is_real)
		{
			s.RemoveAndUnassign( s.GetRequest(next->req_id) );
			prev = s.GetNode( d->last_done_node_id==-1?d->StartNodeID:d->last_done_node_id );
			time = prev->tw_start;
			if(prev->is_fixed_pos)
				time = prev->departure_time;
			if(time < Parameters::GetCurrentTime() && !prev->is_going_to_depot)
				time = Parameters::GetCurrentTime();
			continue;
		}


		if(next->type == NODE_TYPE_PICKUP && next->action_type == DECISION_ACTION_WAIT && time <= Parameters::GetCurrentTime()+ Parameters::GetWaitingTime())
			time = Parameters::GetCurrentTime() + Parameters::GetWaitingTime();

		if(next->type == NODE_TYPE_PICKUP && next->action_type == DECISION_ACTION_GO_NOW && time > Parameters::GetCurrentTime()+0.01)
		{
			s.RemoveAndUnassign( s.GetRequest(next->req_id) );
			prev = s.GetNode( d->last_done_node_id==-1?d->StartNodeID:d->last_done_node_id );
			time = prev->tw_start;
			continue;
		}

		if(time < next->tw_start)
			time = next->tw_start;
		else if(time > next->tw_end+0.0001)
		{

			//Time window violation on a node that we can't remove
			//We find a request that isn't fixed between d->last_done_node_id and that node
			if(next->is_fixed_route || next->type == NODE_TYPE_END_DEPOT)
			{
				Node * cur = prev;
				Node * to_remove = NULL;
				while(cur->id != d->last_done_node_id)
				{
					//if cur can be removed
					if(cur->IsCustomer() && !cur->is_fixed_route && !cur->is_fixed_pos)
					{
						Node * n = s.GetNode( std::max(cur->pick_node_id, cur->del_node_id) );
						if(!n->is_fixed_route && !n->is_fixed_pos)
						{
							to_remove = cur;
							break;
						}
					}

					cur = s.GetPrev(cur);
				}

				if(to_remove != NULL)
				{
					s.RemoveAndUnassign( s.GetRequest(to_remove->req_id) );
					time = start_time;
					prev = s.GetNode( d->last_done_node_id==-1?d->StartNodeID:d->last_done_node_id );
					continue;
				}
				else
				{
					printf("Node:%d couldn't be removed and no predecessor could be as well arrival:%lf CurTime:%.0lf\n", next->id,time, Parameters::GetCurrentTime() );
					for(Node * n = s.GetNode(d->StartNodeID);n->type != NODE_TYPE_END_DEPOT;n = s.GetNext(n))
	   				n->Show();
	   			 s.Show(d);
					Node * nnn = NULL;
					nnn->id = 4;
					exit(1);
				}
			}
			else if(next->IsCustomer() && !next->is_fixed_pos && !next->is_fixed_route)
			{
				s.RemoveAndUnassign( s.GetRequest(next->req_id) );
				time = start_time;
				prev = s.GetNode( d->last_done_node_id==-1?d->StartNodeID:d->last_done_node_id );
				continue;
			}
			else
			{
				printf("Node:%d shouldn't be removed because it is fixed arrival:%lf CurTime:%.0lf\n", next->id,time, Parameters::GetCurrentTime() );
				for(Node * n = s.GetNode(d->StartNodeID);n->type != NODE_TYPE_END_DEPOT;n = s.GetNext(n))
   				n->Show();
   			 s.Show(d);
				Node * nnn = NULL;
				nnn->id = 4;
				exit(1);
			}


		}

		time += next->serv_time;


		prev = next;
	}
}
