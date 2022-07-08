/*
 * Copyright Jean-Francois Cote 2019
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
*/

#ifndef DRIVER_SDDP
#define DRIVER_SDDP

class Driver
{
	public:
		int id;
		int StartNodeID;
		int EndNodeID;
		int capacity;
		double cur_distance;
		double cur_duration;
		int sum_demand;


		int last_done_node_id; //id of the last node that is position is fixed in the route

		Driver():id(-1), StartNodeID(-1), EndNodeID(-1), capacity(999999999),cur_distance(0),sum_demand(0), last_done_node_id(-1){}

		void Show()
		{
			printf("Driver:%d StartNodeID:%d EndNodeID:%d\n", id, StartNodeID, EndNodeID);
		}
};

#endif
