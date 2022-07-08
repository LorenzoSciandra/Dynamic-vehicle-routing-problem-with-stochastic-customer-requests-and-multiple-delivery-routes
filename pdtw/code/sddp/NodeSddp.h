/*
 * Copyright Jean-Francois Cote 2019
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
*/



#ifndef NODE_PDTW
#define NODE_PDTW

#include "../constants.h"
#include <stdio.h>

class Node
{
	public:
		int id;
		int origin_id;
		int demand;
		int distID;
		char type;
		int serv_time;
		double release_time;
		double tw_start;
		double tw_end;
		int pick_node_id;
		int del_node_id;
		int req_id;
      double x;
		double y;
		double arrival_time;
		double departure_time;	//time we are leaving the current node for the next one
		double prev_departure_time; //the time we left from the previous node to arrive at this node
		double slack;


		bool is_real;				//if this node belong to a real request
		bool is_fixed_pos;		//if this node is fixed in its position
		bool is_fixed_route;		//if this node is fixed in its route
		bool is_going_to_depot;	//to identify that after a drop we go to the depot
		int fixed_route_id;		//id of the driver this node is fixed into

		unsigned int is_assigned_to;		//set of bits to identify which drivers can perform this request

		int action_type;			//if an action has to be taken on this node, go now, wait or unassign

		Node():id(-1),origin_id(-1),distID(-1),demand(0),serv_time(0),is_fixed_pos(false),prev_departure_time(0),
				 tw_start(0), tw_end(9999999),pick_node_id(-1), del_node_id(-1),is_fixed_route(false),departure_time(0),
				 x(0),y(0),arrival_time(0),req_id(-1), release_time(0), is_real(true), fixed_route_id(-1),
				 slack(0), is_going_to_depot(false), is_assigned_to(~0), action_type(-1){}


		bool IsCustomer()
		{
			return (type & NODE_TYPE_CUSTOMER) == NODE_TYPE_CUSTOMER;
		}

		bool IsFixed()
		{
			return is_fixed_pos || is_fixed_route;
		}

		void Show()
		{
			if(type == NODE_TYPE_PICKUP)
			{
				printf("Node:%3d OrigId:%3d type:pick x:%4.1lf y:%4.1lf distID:%3d req:%d rel:%3d tw:%3d-%3d drop:%d ", id,origin_id, x,y,distID,req_id,(int)release_time, (int)tw_start,(int)tw_end, del_node_id);
				printf("Arr:%.1lf Dep:%.1lf Real:%d ", arrival_time, departure_time,(int)is_real);
				printf("Fixed:%d FixedRoute:%d action:%d ",(int)is_fixed_pos, (int)is_fixed_route, action_type);
				printf("Assign:%d ", is_assigned_to);
				if(arrival_time > tw_end) printf("\t <------- infeasible tw");
				printf("\n");
			}
			else if (type == NODE_TYPE_DROP)
			{
				printf("Node:%3d OrigId:%3d type:drop x:%4.1lf y:%4.1lf distID:%3d req:%d rel:%3d tw:%3d-%3d pick:%d ", id,origin_id, x,y,distID,req_id, (int)release_time,(int)tw_start,(int)tw_end, pick_node_id);
				printf("Arr:%.1lf Dep:%.1lf Real:%d ", arrival_time, departure_time,(int)is_real);
				printf("Fixed:%d FixedRoute:%d action:%d ",(int)is_fixed_pos, (int)is_fixed_route, action_type);
				if(arrival_time > tw_end) printf("\t <------- infeasible tw");
				printf("\n");
			}
			else if (type == NODE_TYPE_START_DEPOT)
				printf("Node:%3d type:stad distID:%3d lw:%4d uw:%4d x:%4.1lf y:%4.1lf\n", id,distID, (int)tw_start, (int)tw_end,x,y);
			else if (type == NODE_TYPE_END_DEPOT)
				printf("Node:%3d type:endd distID:%3d lw:%4d uw:%4d x:%4.1lf y:%4.1lf\n", id,distID, (int)tw_start, (int)tw_end,x,y);
			else
				printf("Node:%3d type:unkno distID:%3d lw:%4d uw:%4d\n", id,distID, (int)tw_start, (int)tw_end);
		}
};

#endif
