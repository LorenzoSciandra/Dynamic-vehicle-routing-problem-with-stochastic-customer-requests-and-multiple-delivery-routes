
#ifndef DECISION_H
#define DECISION_H

#include <stdio.h>
#include "../constants.h"

#define DECISION_TYPE_ROUTING 1
#define DECISION_TYPE_ASSIGNMENT 2
#define DECISION_TYPE_ACTION 3 //GO Delivery Now, Wait, Don't deliver
#define DECISION_TYPE_GLOBAL 4 // a global action to perform

#define DECISION_SELECTION_MODE_MOST_COMMON 1	//select a request without 100% consensus having the highest consensus
#define DECISION_SELECTION_MODE_LEAST_COMMON 2	//select a request without 100% consensus having the smallest consensus

#define DECISION_ACTION_GO_NOW 1
#define DECISION_ACTION_WAIT 2
#define DECISION_ACTION_DONT_DELIVER 3
#define DECISION_ACTION_DONT_UNASSIGN 4 //a go now or wait decision will have priority over the dont assign decision
#define DECISION_ACTION_NOT_DEFINED 5
#define DECISION_ACTION_ASSIGN_TO 6
#define DECISION_ACTION_DONT_ASSIGN_TO 7


class Decision
{
public:
	Decision() : node_id(-1),req_id(-1), prev_node_id(-1), is_real(true), arrival_time(0),decision_type(0),
					 prev_departure_time(0), departure_time(0), prev_node_type(-1), driver_id(-1),
					 is_going_to_depot(false), friend_id(-1),time_to_next(0),release_time(0), action_type(0),
					 is_still_feasible(true), node_type(-1) {}

	void Show()
	{
		printf("Decision Type:%d ",decision_type);
		if(node_id == -1)
			printf("Req:%2d Type:%s Rel:%3.0lf Real:%d ", req_id,Constants_Get_Node_Type(node_type), release_time, (int)is_real);
		else
			printf("NodeID:%3d Req:%2d Type:%s Rel:%3.0lf Real:%d ",node_id, req_id,Constants_Get_Node_Type(node_type), release_time, (int)is_real);

		if(decision_type == DECISION_TYPE_ROUTING)
		{
			printf("Prev(ID:%3d Type:%s Real:%d ",prev_node_id,Constants_Get_Node_Type(prev_node_type), (int)prev_is_real);
			printf("Dep:%.1lf) ", prev_departure_time);
			printf("Drv:%2d Arr:%.1lf Dep:%.1lf friend:%d ",driver_id,arrival_time,departure_time, friend_id);
			printf("time to next:%.1lf ", time_to_next);
			if(is_going_to_depot) printf(" is_going_to_depot");
		}
		else if(decision_type == DECISION_TYPE_ASSIGNMENT)
		{
			if(driver_id == -1)
				printf("Action:reject");
			else if(action_type == DECISION_ACTION_ASSIGN_TO)
				printf("AssignTo:%d ", driver_id);
			else if(action_type == DECISION_ACTION_DONT_ASSIGN_TO)
				printf("DontAssignTo:%d ", driver_id);

		}
		else if(decision_type == DECISION_TYPE_ACTION)
		{
			printf("Action Type:");
			switch(action_type) {
				case DECISION_ACTION_GO_NOW : printf("go now");break;
				case DECISION_ACTION_WAIT : printf("wait");break;
				case DECISION_ACTION_DONT_DELIVER : printf("Don't deliver");break;
				default : printf("unknown");break;
			}
		}
		else if(decision_type == DECISION_TYPE_GLOBAL && action_type == DECISION_ACTION_WAIT)
		{
			printf("Action Type:wait Duration:%.1lf ", departure_time-arrival_time);
		}
		printf("\n");
	}

	bool IsForCustomer()
	{
		return (node_type & NODE_TYPE_CUSTOMER) == NODE_TYPE_CUSTOMER;
	}

	int decision_type;
	int node_id;
	int node_type;
	double release_time;
	int req_id;
	int prev_node_id;			//ID of its previous node, -1 it is the depot
	int prev_node_type;		//type of the previous node
	int next_node_type;		//type of the next node
	bool prev_is_real;
	bool next_is_real;
	int driver_id;				//ID of the driver, -1 if it is unassigned
	bool is_real;
	double arrival_time;				//time we arrive at this node
	double prev_departure_time;	//time we left from the previous node to arrive at this node
	double next_arrival_time;
	double departure_time;			//time we leave the node to go to the next node
	bool is_going_to_depot;
	int friend_id;
	double time_to_next;
	int action_type;
	bool is_still_feasible; //if current_time + distance to node < upper tw
};

struct DecisionSorter
{
	bool operator() (const Decision * d1,const Decision * d2)
	{
		if(d1->driver_id != d2->driver_id)
			return d1->driver_id < d2->driver_id;
		else
			return d1->arrival_time < d2->arrival_time;
	}

	bool operator() (const Decision & d1,const Decision & d2)
	{
		if(d1.driver_id != d2.driver_id)
			return d1.driver_id < d2.driver_id;
		else
			return d1.arrival_time < d2.arrival_time;
	}
};

#endif
