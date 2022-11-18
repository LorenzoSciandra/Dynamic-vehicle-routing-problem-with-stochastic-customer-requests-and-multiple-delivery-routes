
#include "ConsensusSimulation.h"
#include "Decisions.h"
#include "DecisionMultiSet.h"
#include "Solver.h"
#include <time.h>
#include <limits.h>
#include "Parameters.h"
#include "../../../lib/Chrono.h"
#include <stdlib.h>

void ConsensusSimulation::Optimize(Scenarios &scenarios) {
    Parameters::SetSBPA(true);
    srand(Parameters::GetSeed());
    double cur_time = Parameters::GetTimeHorizonStartTime();
    {
        double t = scenarios.GetNextEvent(cur_time);
        if (t > cur_time) //there are no customer at the beginning
            cur_time = t;
    }
    printf("ConsensusSimulation StartTime:%.1lf \n", cur_time);
    ChronoCpuNoStop chrono;
    chrono.start();

    Decisions prev_decisions;
    for (nb_events = 1; cur_time <= Parameters::GetTimeHorizon(); nb_events++) {
        Parameters::SetCurrentTime(cur_time);
        //printf("\n\n\nTime:%.1lf Unassigned:%d\n", cur_time, prev_decisions.GetUnassignedCount());
        //prev_decisions.Show();

        std::vector <Prob<Node, Driver>> probs;
        scenarios.Generate(probs, cur_time);
        DecisionMultiSet dec_consensus;
        for (int i = 0; i < Parameters::GetScenarioCount(); i++) {
            Solver solver(probs[i]);
            solver.SetDecisions(prev_decisions);
            solver.Optimize();
            //printf("Scenario:%d Cost:%.1lf\n", i, solver.cost);
            //solver.Show();
            //solver.ShowCost();

            Decisions decisions;
            solver.GetDecisions(decisions);
            dec_consensus.Add(decisions);
            //solver.Optimize();
            //decisions.Show();
            //decisions.ShowByDriver(8);
        }
        //dec_consensus.Show();

        switch (Parameters::GetConsensusToUse()) {
            case CONSENSUS_STACY:
                dec_consensus.GetConsensusStacy(prev_decisions);
                break;
            case CONSENSUS_BY_DRIVERS:
                dec_consensus.GetConsensusByDrivers(prev_decisions);
                break;
            case CONSENSUS_BY_MINIMAL_CHANGE:
                dec_consensus.GetConsensusMinimalDistance(prev_decisions);
                break;
        }

        //cur_time = scenarios.GetNextEvent(cur_time);
        //prev_decisions.Show();
        cur_time = GetNextEvent(scenarios, prev_decisions, cur_time);
        //printf("Next Time:%.1lf \n", cur_time);

        //if(cur_time >= 75.9) exit(1);

        prev_decisions.Remove(cur_time);
        //printf("\n\n");
    }

    _consensus_to_use = Parameters::GetConsensusToUse();
    _forbid_stochastic_drop_after_real = Parameters::ForbidStochasticDropAfterReal();
    _allow_en_route_returns = Parameters::AllowEnRouteDepotReturns();

    Prob<Node, Driver> real;
    scenarios.GenerateReal(real, Parameters::GetTimeHorizon() + 1);
    Parameters::SetCurrentTime(0);
    Solver solver(real);
    solver.SetDecisions(prev_decisions);
    //solver.Optimize();
    solver.Unfix();
    //solver.Show();
    //prev_decisions.Show();

    solver.Update();
    cost = solver.cost;
    unassigneds = solver.nb_unassigneds / 2;
    distance = solver.distances;
    nb_routes = solver.GetRouteCount();
    time_taken = chrono.getTime();
    Show();


    switch (Parameters::GetConsensusToUse()) {
        case CONSENSUS_STACY:
            final_solution.name = "Consensus-RS";
            break;
        case CONSENSUS_BY_DRIVERS:
            final_solution.name = "Consensus-AS";
            break;
        case CONSENSUS_BY_MINIMAL_CHANGE:
            final_solution.name = "Consensus-ED";
            break;
    }
    if (!_forbid_stochastic_drop_after_real)
        final_solution.name.append(" WS");
    if (_allow_en_route_returns)
        final_solution.name.append(" PDR");
    solver.GetSolutionCompact(final_solution);
    Parameters::SetSBPA(false);
    solver.GetReport(report);
}


//	Types of event :
//	1) A new request arrived
//	2) A driver arrives at the depot
// 3) a driver has finished its waiting period at the depot
double ConsensusSimulation::GetNextEvent(Scenarios &scenarios, Decisions &decs, double cur_time) {
    double next_request = scenarios.GetNextEvent(cur_time);
    double ne = decs.GetNextEvent(scenarios.real);
    return std::min(ne, next_request);

/*
	double next_request = scenarios.GetNextEvent(cur_time);
	double next_arrival_depot = Parameters::GetTimeHorizon()+99;
	double next_wait = Parameters::GetTimeHorizon()+99;
	double next_stochastic_arrival_depot = Parameters::GetTimeHorizon()+99;

	for(int i=0;i<decs.GetCount();i++)
	{
		Decision * d = decs.Get(i);
		if(d->decision_type != DECISION_TYPE_ROUTING) continue;

		if(d->node_type == NODE_TYPE_PICKUP && !d->is_real && d->arrival_time > cur_time)
			next_stochastic_arrival_depot = std::min(next_stochastic_arrival_depot, d->arrival_time);

		if(!d->is_real) continue;
*/
    /*if(d->node_type == NODE_TYPE_DROP && d->next_node_type != NODE_TYPE_DROP &&
        //d->departure_time < cur_time &&
        d->departure_time + d->time_to_next > cur_time)
        next_arrival_depot = std::min(next_arrival_depot, d->departure_time + d->time_to_next);

    if( ((d->node_type == NODE_TYPE_PICKUP && d->next_node_type == NODE_TYPE_DROP) ||
          (d->is_going_to_depot)) &&
            d->departure_time < cur_time && d->departure_time + d->time_to_next < d->next_arrival_time &&
            d->next_arrival_time > cur_time)
        {
            double next = d->departure_time + d->time_to_next; //
            for(;next-0.01 < cur_time;next++);
            next_wait = std::min(next_wait,next);
        }*/
/*
		if(d->node_type == NODE_TYPE_PICKUP && d->arrival_time > cur_time)
			next_arrival_depot = std::min(next_arrival_depot, d->arrival_time);




		if(d->is_going_to_depot && d->departure_time + d->time_to_next > cur_time)
			next_arrival_depot = std::min(next_arrival_depot, d->departure_time + d->time_to_next);


		if( (d->node_type == NODE_TYPE_PICKUP || d->is_going_to_depot) && d->arrival_time <= cur_time && (d->next_arrival_time > cur_time || !d->next_is_real))
		{
			double next = d->arrival_time;
			if(d->is_going_to_depot) next = d->departure_time + d->time_to_next;
			for(;next-0.01 < cur_time;next++);
			next_wait = std::min(next_wait,next);
		}
	}

	//printf("Current time:%.1lf next_request:%.1lf ",cur_time,next_request);
	//printf("next_arrival_depot:%.1lf next_sto_arrival_depot:%.1lf next_wait:%.1lf\n",next_arrival_depot, next_stochastic_arrival_depot,next_wait);



	double ne = decs.GetNextEvent();
	return std::min(ne, next_request);
	//double mm = std::min( std::min(next_request, next_arrival_depot), next_wait);
	//return std::min(next_stochastic_arrival_depot, std::max(cur_time+1, mm));
	//return mm;
*/
}
