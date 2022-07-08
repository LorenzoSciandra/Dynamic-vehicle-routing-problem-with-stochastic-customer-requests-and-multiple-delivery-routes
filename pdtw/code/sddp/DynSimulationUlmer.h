#ifndef DYNSIMULATION_ULMER_H
#define DYNSIMULATION_ULMER_H

#include "Scenarios.h"
#include "Decisions.h"
#include "Parameters.h"

class DynSimulationUlmer
{
public:

	double cost;
	double time_taken;
	int unassigneds;
	double distance;
	int nb_routes;
	int nb_events;
	int future_assignments;
	int future_customers;
	int known_unassignments;

	DynSimulationUlmer(){}


	void Optimize(Scenarios & scenarios);

	void Show()
	{
		printf("Dynamic ");
		printf("Cost:%.2lf Dist:%.2lf Un:%d Routes:%d ", cost, distance, unassigneds, nb_routes);
		printf("Time:%.3lf Events:%d\n", time_taken,nb_events);
	}

private:
	double GetNextEvent(Scenarios & scenarios, Decisions & decs, double cur_time);
};


#endif
