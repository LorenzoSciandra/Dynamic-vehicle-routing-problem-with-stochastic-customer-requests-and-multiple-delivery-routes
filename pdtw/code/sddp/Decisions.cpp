
#include "../constants.h"
#include "Decisions.h"
#include "Parameters.h"
#include "stdlib.h"
#include "stdio.h"
#include <algorithm>
#include <map>
#include <cmath>

void Decisions::Remove(double time)
{
	//printf("Remove:%.2lf\n", time);
	std::map<int, bool> pickup_made;
	std::vector<Decision> decs;
	for(size_t i=0;i<decisions.size();i++)
	{
		if(decisions[i].decision_type == DECISION_TYPE_ROUTING && decisions[i].is_real)
		{
			if(decisions[i].node_type == NODE_TYPE_PICKUP && decisions[i].arrival_time < time)
			{
				pickup_made[ decisions[i].node_id ] = true;
				decs.push_back( decisions[i] );
			}
			else if(decisions[i].node_type == NODE_TYPE_DROP && pickup_made[ decisions[i].friend_id ])
				decs.push_back( decisions[i] );
			else if(decisions[i].node_type == NODE_TYPE_START_DEPOT)
				decs.push_back( decisions[i] );
		}
		else if(decisions[i].decision_type == DECISION_TYPE_ASSIGNMENT)
			decs.push_back( decisions[i] );
		else if(decisions[i].decision_type == DECISION_TYPE_GLOBAL)
			decs.push_back( decisions[i] );

	}

	decisions = decs;
}

void Decisions::FillMap(std::map<int, Decision*> & map_dec, int type)
{
	map_dec.clear();
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].decision_type == type)
			map_dec[ decisions[i].node_id ] = &decisions[i];

}
void Decisions::FillDecisionsByDrivers()
{
	drivers_decisions.clear();
	drivers_decisions.resize(Parameters::GetDriverCount());

	for(size_t i=0;i<decisions.size();i++)
	{
		Decision * d = Get(i);
		if(d->decision_type != DECISION_TYPE_ROUTING) continue;
		if(d->driver_id == -1) continue;
		drivers_decisions[d->driver_id].push_back(d);
	}

	DecisionSorter sorter;
	for(size_t i=0;i<drivers_decisions.size();i++)
		std::sort(drivers_decisions[i].begin(), drivers_decisions[i].end(), sorter);

	//printf("Decision Drivers:");
	//for(size_t i=0;i<drivers_decisions.size();i++)
	//	printf("%d ", (int)drivers_decisions[i].size());
	//printf("\n");
}
bool Decisions::IsDriverAtDepot(int drv)
{
	if(drivers_decisions[drv].size() == 0) return true;

	for(size_t k=0;k<drivers_decisions[drv].size();k++)
	{
		Decision * d = drivers_decisions[drv][k];
		if(d->node_type == NODE_TYPE_PICKUP && d->arrival_time <= Parameters::GetCurrentTime() &&
			d->departure_time > Parameters::GetCurrentTime())
		{
			//printf("\n1 DecisioNo:%d Driver:%d at depot: ", (int)k,drv); d->Show();
			return true;
		}
		else if(d->node_type == NODE_TYPE_DROP && d->is_going_to_depot &&
				  d->departure_time < Parameters::GetCurrentTime() &&
				  d->departure_time+d->time_to_next == Parameters::GetCurrentTime())
		{
			//printf("\n2 DecisioNo:%d Driver:%d at depot: ", (int)k,drv); d->Show();
			return true;
		}
		else if(d->node_type == NODE_TYPE_START_DEPOT &&
				k+1 < drivers_decisions[drv].size() &&
				d->departure_time <= Parameters::GetCurrentTime() &&
				d->departure_time+d->time_to_next < drivers_decisions[drv][k+1]->arrival_time)
			return true;
	}

	Decision * d = drivers_decisions[drv].back();
	if(d->node_type == NODE_TYPE_DROP && d->is_going_to_depot &&
		d->departure_time+d->time_to_next <= Parameters::GetCurrentTime())
	{
		//printf("\n3 Driver:%d at depot: ", drv); d->Show();
		return true;
	}
	else if(d->node_type == NODE_TYPE_START_DEPOT)
		return true;
	else
		return false;
}

double Decisions::GetNextEvent()
{
	double cur_time = Parameters::GetCurrentTime();
	FillDecisionsByDrivers();
	/*printf("Drivers at depot:");
	for(int i=0;i<Parameters::GetDriverCount();i++)
		if(IsDriverAtDepot(i))
			printf("%d ", i);
	printf("\n");*/
	int max_req_id = -1;
	for(size_t j=0;j<decisions.size();j++)
		if(decisions[j].is_real)
			max_req_id = std::max(max_req_id, decisions[j].req_id);
	max_req_id++;

	bool has_free_request = false;
	bool has_vehicle_at_depot = false;
	double next_stochastic_pickup_depot = Parameters::GetTimeHorizon()+100;
	double next_arrival_depot = Parameters::GetTimeHorizon()+100;
	double next_wait = Parameters::GetTimeHorizon()+100;

	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].decision_type == DECISION_TYPE_GLOBAL && decisions[i].action_type == DECISION_ACTION_WAIT)
			next_wait = std::min(next_wait, decisions[i].departure_time);

	for(int i=0;i<Parameters::GetDriverCount();i++)
	{
		bool vehicle_at_depot = IsDriverAtDepot(i);
		has_vehicle_at_depot = has_vehicle_at_depot | vehicle_at_depot;

		if(vehicle_at_depot && (Parameters::IsSBPA() || (Parameters::IsBranchAndRegret() && !Parameters::EvaluateWaitingStrategy())))
			next_wait = cur_time+1;

		bool found_stoc = false;
		for(size_t j=0;j<drivers_decisions[i].size();j++)
		{
			Decision * d = drivers_decisions[i][j];
			if(d->decision_type != DECISION_TYPE_ROUTING) continue;

			if(d->driver_id == -1 && d->is_real && d->node_type == NODE_TYPE_PICKUP && d->is_still_feasible) has_free_request = true;
			if(d->driver_id == -1) continue;

			if(d->node_type == NODE_TYPE_PICKUP && !d->is_real)
				next_stochastic_pickup_depot = std::min(next_stochastic_pickup_depot, std::max(d->arrival_time, d->release_time) );

			if(d->is_real && d->node_type == NODE_TYPE_PICKUP && d->arrival_time > cur_time)
				next_arrival_depot = std::min(next_arrival_depot, d->arrival_time);

			if(d->is_real && d->is_going_to_depot && d->departure_time + d->time_to_next > cur_time)
				next_arrival_depot = std::min(next_arrival_depot, d->departure_time + d->time_to_next);

			if(!d->is_real) found_stoc = true;
			if(d->is_real && found_stoc) has_free_request = true;
			if(d->is_real && d->node_type == NODE_TYPE_PICKUP && d->arrival_time > cur_time)has_free_request = true;

			if(!Parameters::EvaluateWaitingStrategy() &&
				d->is_real &&
				vehicle_at_depot &&
				(d->node_type == NODE_TYPE_PICKUP || d->is_going_to_depot) &&
				d->arrival_time <= cur_time && (d->next_arrival_time > cur_time || !d->next_is_real))
			{
				double next = d->arrival_time;
				if(d->is_going_to_depot) next = d->departure_time + d->time_to_next;
				for(;next-0.01 < cur_time+1;next++);
				next_wait = std::min(next_wait,next);
			}
		}
	}
	//printf("GetNextEvent free_request:%d vehicle at depot:%d next_wait:%.1lf ",(int)has_free_request,(int)has_vehicle_at_depot,next_wait);
	//printf("next stoc_pickup:%.1lf next_arrival:%.1lf\n", next_stochastic_pickup_depot, next_arrival_depot);

	if(has_free_request && has_vehicle_at_depot)
		return std::min(next_stochastic_pickup_depot, next_wait);
	else if(has_free_request)
		return std::min(next_stochastic_pickup_depot, next_arrival_depot);
	else
		return Parameters::GetTimeHorizon()+1;
}

void Decisions::Show()
{
	for(size_t i=0;i<decisions.size();i++)
		decisions[i].Show();
}

void Decisions::ShowOnlyReal()
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].is_real)
			decisions[i].Show();
}
void Decisions::ShowOnlyRealCustomer()
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].is_real && (decisions[i].node_type == NODE_TYPE_PICKUP || decisions[i].node_type == NODE_TYPE_DROP))
			decisions[i].Show();
}
void Decisions::ShowOnlyForRequest(int req_id)
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].req_id == req_id)
			decisions[i].Show();
}
void Decisions::ShowByDriver(int id)
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].driver_id == id)
			decisions[i].Show();
}
void Decisions::ShowByAssignments()
{
	for(int i=0;i<Parameters::GetDriverCount();i++)
	{
		printf("Driver:%d Nodes:", i);
		for(size_t j=0;j<decisions.size();j++)
			if(decisions[j].driver_id == i)
				printf("%d ", decisions[j].node_id);
		printf("\n");
	}
	printf("Un:");
	for(size_t j=0;j<decisions.size();j++)
		if(decisions[j].driver_id == -1)
			printf("%d ", decisions[j].node_id);
	printf("\n");
}

void Decisions::Sort()
{
	DecisionSorter decisionsorter;
	std::sort(decisions.begin(), decisions.end(), decisionsorter);
}

bool Decisions::HasDriverAtDepot()
{
	FillDecisionsByDrivers();
	for(int i=0;i<Parameters::GetDriverCount();i++)
		if(IsDriverAtDepot(i))
			return true;
	return false;
}


int Decisions::GetUnassignedCount()
{
	int cnt = 0;
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].driver_id == -1 && decisions[i].node_type == NODE_TYPE_PICKUP && decisions[i].is_real)
			cnt++;
	return cnt;
}

bool Decisions::HasDecisionReqDriver(int req_id, int driver_id)
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].driver_id == driver_id && decisions[i].req_id == req_id)
			return true;
	return false;
}

bool Decisions::HasReqBeenPerformed(int req_id)
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].req_id == req_id && decisions[i].driver_id != -1 && decisions[i].arrival_time < Parameters::GetCurrentTime() &&
		decisions[i].decision_type == DECISION_TYPE_ROUTING)
			return true;
	return false;
}

bool Decisions::HasDecisionReal(int req_id)
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].req_id == req_id && decisions[i].is_real)
			return true;
	return false;
}

bool Decisions::HasAssignmentDecisionReq(int req_id)
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].req_id == req_id && decisions[i].decision_type == DECISION_TYPE_ASSIGNMENT)
			return true;
	return false;
}

bool Decisions::HasAssignmentDecisionReq(int req_id, int action_type, int driver_id)
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].req_id == req_id && decisions[i].decision_type == DECISION_TYPE_ASSIGNMENT &&
			decisions[i].action_type == action_type && decisions[i].driver_id == driver_id)
			return true;
	return false;
}

bool Decisions::HasActionDecisionReq(int req_id)
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].req_id == req_id && ((decisions[i].decision_type == DECISION_TYPE_ASSIGNMENT && decisions[i].driver_id == -1) ||
														 (decisions[i].decision_type == DECISION_TYPE_ACTION)))
			return true;
	return false;
}

bool Decisions::HasActionDecisionReqExceptDontUnassign(int req_id)
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].req_id == req_id && ((decisions[i].decision_type == DECISION_TYPE_ASSIGNMENT && decisions[i].driver_id == -1) ||
														 (decisions[i].decision_type == DECISION_TYPE_ACTION && decisions[i].action_type != DECISION_ACTION_DONT_UNASSIGN)))
			return true;
	return false;
}

bool Decisions::HasDecisionCharacteristics(Decision * d)
{
	if(d->decision_type == DECISION_TYPE_ASSIGNMENT)
	{
		for(size_t i=0;i<decisions.size();i++)
			if(d->req_id  == decisions[i].req_id && decisions[i].decision_type == DECISION_TYPE_ROUTING &&
				( (d->action_type == DECISION_ACTION_ASSIGN_TO && decisions[i].driver_id == d->driver_id) ||
				  (d->action_type == DECISION_ACTION_DONT_ASSIGN_TO && decisions[i].driver_id != d->driver_id)))
				return true;
		return false;
	}
	else if(d->decision_type == DECISION_TYPE_ACTION)
	{
		for(size_t i=0;i<decisions.size();i++)
			if(d->req_id  == decisions[i].req_id &&
				decisions[i].decision_type == DECISION_TYPE_ROUTING &&
				decisions[i].node_type == NODE_TYPE_PICKUP)
			{
				if(abs(decisions[i].arrival_time - Parameters::GetCurrentTime()) < 0.001 && (d->action_type == DECISION_ACTION_GO_NOW || d->action_type == DECISION_ACTION_DONT_UNASSIGN))
					return true;
				if(decisions[i].arrival_time > Parameters::GetCurrentTime()+0.001 && d->action_type == DECISION_ACTION_WAIT)
					return true;
			}
		return false;
	}
	else
	{
		printf("This type of decision wasn't implemented in HasDecisionCharacteristics()\n");
		d->Show();
		exit(1);
	}
	return false;
}



Decision * Decisions::GetByRequestID(int req_id)
{
	for(size_t i=0;i<decisions.size();i++)
		if(decisions[i].req_id == req_id)
			return &decisions[i];
	return NULL;
}
