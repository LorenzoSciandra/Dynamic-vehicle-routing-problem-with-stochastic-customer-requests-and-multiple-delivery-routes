
#include "DecisionMultiSet.h"
#include "Scenarios.h"
#include "Parameters.h"
#include <vector>
#include <algorithm>
#include <stdio.h>
#include "../../../lib/SolveAssignmentProblem.h"
#include <cmath>

using namespace AssignmentProblemSolver;

bool DecisionMultiSet::operator<(DecisionMultiSet &other) {
    return GetAverageCost() < other.GetAverageCost();
}

bool DecisionMultiSet::operator>(DecisionMultiSet &other) {
    return GetAverageCost() > other.GetAverageCost();
}

void DecisionMultiSet::GetConsensusByDrivers(Decisions &decisions) {
    //printf("GetConsensus\n");
    int best_score = -1;
    int best_report = 0;
    int req_id, driver_id;

    for (int i = 0; i < reports.size(); i++) {
        int new_score = 0;
        int assignments = 0, unassigneds = 0;
        for (int k = 0; k < reports[i].GetCount(); k++) {
            if (reports[i].Get(k)->node_type != NODE_TYPE_DROP) continue;
            if (!reports[i].Get(k)->is_real) continue;

            req_id = reports[i].Get(k)->req_id;
            driver_id = reports[i].Get(k)->driver_id;

            if (req_id != -1 && driver_id != -1) assignments++;
            if (req_id != -1 && driver_id == -1) unassigneds++;

            for (int l = 0; l < reports.size(); l++)
                if (l != i) {
                    if (reports[l].HasDecisionReqDriver(req_id, driver_id) == true) {
                        new_score++;
                        //printf("Report %d  origin Client id: %d, report match %d\n", i, req_id, l);
                    }
                }
        }
        //printf("Report:%d	Score:%d Assignments:%d Un:%d\n", i, new_score, assignments, unassigneds);

        if (new_score > best_score) {
            best_score = new_score;
            best_report = i;
        }
    }
    if (best_score >= 0)
        decisions = reports[best_report];
    //printf("Best Report: %d\n", best_report);
    //decisions.Show();
}

void DecisionMultiSet::GetConsensusStacy(Decisions &decisions) {
    for (size_t i = 0; i < reports.size(); i++)
        reports[i].FillDecisionsByDrivers();

    std::vector<std::vector<std::vector<int> > > routes_sets(reports.size());

    for (size_t i = 0; i < reports.size(); i++) {
        routes_sets[i].resize(Parameters::GetDriverCount());

        std::vector<bool> driver_stop_adding(Parameters::GetDriverCount(), false);
        std::vector<bool> driver_at_depot(Parameters::GetDriverCount(), false);
        for (int k = 0; k < Parameters::GetDriverCount(); k++)
            driver_at_depot[k] = reports[i].IsDriverAtDepot(k);

        //printf("Report:%d drivers at depot:", (int)i);
        //for(size_t k=0;k<driver_at_depot.size();k++)
        //	if(driver_at_depot[k])
        //		printf("%d ", (int)k);
        //printf("\n");

        for (int k = 0; k < reports[i].GetCount(); k++) {
            Decision *d = reports[i].Get(k);
            if (d->arrival_time < Parameters::GetCurrentTime()) continue;
            if (d->driver_id == -1) continue;
            if (!driver_at_depot[d->driver_id]) continue;
            if (!d->is_real) driver_stop_adding[d->driver_id] = true;
            if (driver_stop_adding[d->driver_id]) continue;

            if (!Parameters::AllowEnRouteDepotReturns()) {
                if (routes_sets[i][d->driver_id].size() >= 1 && d->node_type == NODE_TYPE_PICKUP)
                    driver_stop_adding[d->driver_id] = true;
            } else {
                if (routes_sets[i][d->driver_id].size() >= 1 && routes_sets[i][d->driver_id].back() != -1 &&
                    d->node_type == NODE_TYPE_PICKUP)
                    routes_sets[i][d->driver_id].push_back(-1);
            }
            if (driver_stop_adding[d->driver_id]) continue;

            if (d->node_type == NODE_TYPE_DROP)
                routes_sets[i][d->driver_id].push_back(d->node_id);
        }
    }

    for (size_t i = 0; i < routes_sets.size(); i++)
        for (int j = 0; j < Parameters::GetDriverCount(); j++)
            while (routes_sets[i][j].size() >= 1 && routes_sets[i][j].back() == -1)
                routes_sets[i][j].pop_back();


    for (size_t i = 0; i < routes_sets.size(); i++)
        for (size_t j = 0; j < routes_sets[i].size(); j++)
            if (routes_sets[i][j].size() >= 2) {
                //check if there is a return to the depot, if so, don't sort
                bool has_return = false;
                for (size_t k = 0; k < routes_sets[i][j].size(); k++)
                    if (routes_sets[i][j][k] == -1) {
                        has_return = true;
                        break;
                    }
                if (!has_return)
                    std::sort(routes_sets[i][j].begin(), routes_sets[i][j].end());
            }

    int best_report = -1;
    int best_identicals = -1;
    for (size_t i = 0; i < reports.size(); i++) {
        int nb_identicals = 0; //number of times the exact same routes are in other reports
        int nb_routes = 0; //number of routes in the current report
        for (size_t j = 0; j < routes_sets[i].size(); j++)
            if (routes_sets[i][j].size() >= 1)
                nb_routes++;

        //printf("Report:%d identical_to:", (int)i);
        for (size_t k = 0; k < reports.size(); k++)
            if (k != i) {
                int nbtimes = 0;
                for (size_t j = 0; j < routes_sets[i].size(); j++) {
                    if (routes_sets[i][j].size() == 0) continue;

                    for (size_t l = 0; l < routes_sets[k].size(); l++)
                        if (routes_sets[i][j].size() == routes_sets[k][l].size() &&
                            routes_sets[i][j] == routes_sets[k][l]) {
                            nbtimes++;
                            break;
                        }
                }
                if (nbtimes == nb_routes && nb_routes >= 1)
                    nb_identicals++;
            }

        if (nb_identicals > best_identicals) {
            best_report = (int) i;
            best_identicals = nb_identicals;
        }

        /*
        printf("Report:%d NbIdenticals:%d NbRoutes:%d\n", (int)i,nb_identicals,nb_routes);
        for(size_t k=0;k<routes_sets[i].size();k++)
        {
            if(routes_sets[i][k].size() == 0) continue;

            printf("Driver:%d Route:", (int)k);
            for(size_t l=0;l<routes_sets[i][k].size();l++)
                printf("%d ", routes_sets[i][k][l]);
            printf("\n");
        }*/
    }
    if (best_report >= 0)
        decisions = reports[best_report];
    //printf("Best Report: %d\n", best_report);
}

void DecisionMultiSet::GetConsensusMinimalDistance(Decisions &decisions) {
    for (size_t i = 0; i < reports.size(); i++)
        reports[i].FillDecisionsByDrivers();

    std::vector<std::vector<std::vector<int> > > routes_sets(reports.size());

    for (size_t i = 0; i < reports.size(); i++) {
        routes_sets[i].resize(Parameters::GetDriverCount());

        std::vector<bool> driver_stop_adding(Parameters::GetDriverCount(), false);
        std::vector<bool> driver_at_depot(Parameters::GetDriverCount(), false);
        for (int k = 0; k < Parameters::GetDriverCount(); k++)
            driver_at_depot[k] = reports[i].IsDriverAtDepot(k);

        //printf("Report:%d drivers at depot:", (int)i);
        //for(size_t k=0;k<driver_at_depot.size();k++)
        //	if(driver_at_depot[k])
        //		printf("%d ", (int)k);
        //printf("\n");

        for (int k = 0; k < reports[i].GetCount(); k++) {
            Decision *d = reports[i].Get(k);
            if (d->arrival_time < Parameters::GetCurrentTime()) continue;
            if (d->driver_id == -1) continue;
            if (!driver_at_depot[d->driver_id]) continue;
            if (!d->is_real) driver_stop_adding[d->driver_id] = true;
            if (driver_stop_adding[d->driver_id]) continue;

            if (!Parameters::AllowEnRouteDepotReturns()) {
                if (routes_sets[i][d->driver_id].size() >= 1 && d->node_type == NODE_TYPE_PICKUP)
                    driver_stop_adding[d->driver_id] = true;
            } else {
                if (routes_sets[i][d->driver_id].size() >= 1 && routes_sets[i][d->driver_id].back() != -1 &&
                    d->node_type == NODE_TYPE_PICKUP)
                    routes_sets[i][d->driver_id].push_back(-1);
            }
            if (driver_stop_adding[d->driver_id]) continue;

            if (d->node_type == NODE_TYPE_DROP)
                routes_sets[i][d->driver_id].push_back(d->node_id);
        }
    }

    for (size_t i = 0; i < routes_sets.size(); i++)
        for (int j = 0; j < Parameters::GetDriverCount(); j++)
            while (routes_sets[i][j].size() >= 1 && routes_sets[i][j].back() == -1)
                routes_sets[i][j].pop_back();


    for (size_t i = 0; i < routes_sets.size(); i++)
        for (size_t j = 0; j < routes_sets[i].size(); j++)
            if (routes_sets[i][j].size() >= 2) {
                //check if there is a return to the depot, if so, don't sort
                bool has_return = false;
                for (size_t k = 0; k < routes_sets[i][j].size(); k++)
                    if (routes_sets[i][j][k] == -1) {
                        has_return = true;
                        break;
                    }
                if (!has_return)
                    std::sort(routes_sets[i][j].begin(), routes_sets[i][j].end());
            }

    /*for(size_t i=0;i<routes_sets.size();i++)
    {
        printf("Report:%d\n", (int)i);
        for(size_t j=0;j<routes_sets[i].size();j++)
        {
            printf("\t Driver:%d route:", (int)j);
            for(size_t k=0;k<routes_sets[i][j].size();k++)
                printf("%d", routes_sets[i][j][k]);
            printf("\n");
        }
    }*/

    int best_report = -1;
    int best_score = 9999;
    for (size_t i = 0; i < reports.size(); i++) {
        int report_score = 0;
        for (size_t j = 0; j < reports.size(); j++) {
            if (i == j) continue;

            std::vector<std::vector<int>> cost_trans_matrix(Parameters::GetDriverCount());
            for (int k = 0; k < Parameters::GetDriverCount(); k++)
                cost_trans_matrix[k].resize(Parameters::GetDriverCount());

            for (int k = 0; k < Parameters::GetDriverCount(); k++)
                for (int l = 0; l < Parameters::GetDriverCount(); l++) {
                    int score = 0;

                    for (int r1 = 0; r1 < routes_sets[i][k].size(); r1++) {
                        bool is_in = false;
                        for (int r2 = 0; r2 < routes_sets[j][l].size(); r2++)
                            if (routes_sets[i][k][r1] == routes_sets[j][l][r2]) {
                                is_in = true;
                                break;
                            }
                        if (!is_in) score++;
                    }

                    for (size_t r2 = 0; r2 < routes_sets[j][l].size(); r2++) {
                        bool is_in = false;
                        for (size_t r1 = 0; r1 < routes_sets[i][k].size(); r1++)
                            if (routes_sets[i][k][r1] == routes_sets[j][l][r2]) {
                                is_in = true;
                                break;
                            }
                        if (!is_in) score++;
                    }

                    cost_trans_matrix[k][l] = score;
                }
            //printf("report i:%d j:%d\n", (int)i,(int)j);
            //PrintMatrix(cost_trans_matrix);

            Result re = Solve(cost_trans_matrix, MODE_MINIMIZE_COST);
            report_score += re.totalCost;
            //printf("Report i:%d j:%d Score:%d\n",(int)i,(int)j, re.totalCost);
            //getchar();
        }


        if (report_score < best_score) {
            best_report = (int) i;
            best_score = report_score;
        }
        //printf("Report:%d score:%d\n", (int)i,report_score);

        for (size_t k = 0; k < routes_sets[i].size(); k++) {
            if (routes_sets[i][k].size() == 0) continue;

            //	printf("Driver:%d Route:", (int)k);
            //	for(size_t l=0;l<routes_sets[i][k].size();l++)
            //		printf("%d ", routes_sets[i][k][l]);
            //	printf("\n");
        }
    }
    if (best_report >= 0)
        decisions = reports[best_report];
    //printf("Best Report: %d\n", best_report);
    //getchar();
    //if(best_score >= 13) exit(1);
}

int DecisionMultiSet::GetConsensusLevel() {
    printf("GetConsensusLevel()\n");
    for (size_t i = 0; i < reports.size(); i++)
        reports[i].FillDecisionsByDrivers();

    std::vector<std::vector<std::vector<int> > > routes_sets(reports.size());

    for (size_t i = 0; i < reports.size(); i++) {
        routes_sets[i].resize(Parameters::GetDriverCount());

        std::vector<bool> driver_stop_adding(Parameters::GetDriverCount(), false);
        std::vector<bool> driver_at_depot(Parameters::GetDriverCount(), false);
        for (int k = 0; k < Parameters::GetDriverCount(); k++)
            driver_at_depot[k] = reports[i].IsDriverAtDepot(k);

        printf("Report:%d drivers at depot:", (int) i);
        for (size_t k = 0; k < driver_at_depot.size(); k++)
            if (driver_at_depot[k])
                printf("%d ", (int) k);
        printf("\n");

        for (int k = 0; k < reports[i].GetCount(); k++) {
            Decision *d = reports[i].Get(k);
            if (d->arrival_time < Parameters::GetCurrentTime()) continue;
            if (d->driver_id == -1) continue;
            if (!driver_at_depot[d->driver_id]) continue;
            if (!d->is_real) driver_stop_adding[d->driver_id] = true;
            if (routes_sets[i][d->driver_id].size() >= 1 && d->node_type == NODE_TYPE_PICKUP)
                driver_stop_adding[d->driver_id] = true;
            if (driver_stop_adding[d->driver_id]) continue;

            if (d->node_type == NODE_TYPE_DROP)
                routes_sets[i][d->driver_id].push_back(d->node_id);
        }
    }

    for (size_t i = 0; i < routes_sets.size(); i++)
        for (size_t j = 0; j < routes_sets[i].size(); j++)
            if (routes_sets[i][j].size() >= 2)
                std::sort(routes_sets[i][j].begin(), routes_sets[i][j].end());

    int best_report = -1;
    int best_identicals = -1;
    for (size_t i = 0; i < reports.size(); i++) {
        int nb_identicals = 0; //number of times the exact same routes are in other reports
        int nb_routes = 0; //number of routes in the current report
        for (size_t j = 0; j < routes_sets[i].size(); j++)
            if (routes_sets[i][j].size() >= 1)
                nb_routes++;

        //printf("Report:%d identical_to:", (int)i);
        for (size_t k = 0; k < reports.size(); k++)
            if (k != i) {
                int nbtimes = 0;
                for (size_t j = 0; j < routes_sets[i].size(); j++) {
                    if (routes_sets[i][j].size() == 0) continue;

                    for (size_t l = 0; l < routes_sets[k].size(); l++)
                        if (routes_sets[i][j].size() == routes_sets[k][l].size() &&
                            routes_sets[i][j] == routes_sets[k][l]) {
                            nbtimes++;
                            break;
                        }
                }
                if (nbtimes == nb_routes && nb_routes >= 1)
                    nb_identicals++;
            }

        if (nb_identicals > best_identicals) {
            best_report = (int) i;
            best_identicals = nb_identicals;
        }

        printf("Report:%d NbIdenticals:%d NbRoutes:%d\n", (int) i, nb_identicals, nb_routes);
        for (size_t k = 0; k < routes_sets[i].size(); k++) {
            if (routes_sets[i][k].size() == 0) continue;

            printf("Driver:%d Route:", (int) k);
            for (size_t l = 0; l < routes_sets[i][k].size(); l++)
                printf("%d ", routes_sets[i][k][l]);
            printf("\n");
        }
    }
    printf("ConsensusLevel:%d\n", best_identicals);
    if (Parameters::GetCurrentTime() >= 48.9) exit(1);
    return best_identicals;
}


void DecisionMultiSet::Show() {
    printf("DecisionMultiSet::Show()\n");
    int max_req_id = -1;
    for (int i = 0; i < reports.size(); i++)
        for (int k = 0; k < reports[i].GetCount(); k++) {
            if (!reports[i].Get(k)->is_real) continue;
            max_req_id = std::max(max_req_id, reports[i].Get(k)->req_id);
        }

    std::vector<std::vector<int> > assignments(max_req_id + 1);
    for (int i = 0; i < assignments.size(); i++)
        assignments[i].resize(Parameters::GetDriverCount() + 1);

    for (size_t i = 0; i < reports.size(); i++) {
        //printf("\nReport:%zu\n", i);
        //for(int k=0;k < reports[i].GetCount(); k++)
        //	if(reports[i].Get(k)->node_type == NODE_TYPE_DROP)
        //		printf("Client:%d	Driver:%d\n",reports[i].Get(k)->req_id, reports[i].Get(k)->driver_id);

        std::vector<std::vector<int> > routes(Parameters::GetDriverCount() + 1);
        std::vector<int> unassigned;


        for (int k = 0; k < reports[i].GetCount(); k++) {
            if (reports[i].Get(k)->node_type != NODE_TYPE_DROP) continue;
            if (!reports[i].Get(k)->is_real) continue;

            assignments[reports[i].Get(k)->req_id][reports[i].Get(k)->driver_id + 1]++;

            if (reports[i].Get(k)->driver_id != -1)
                routes[reports[i].Get(k)->driver_id].push_back(reports[i].Get(k)->req_id);
            else
                unassigned.push_back(reports[i].Get(k)->req_id);
        }


        /*for(size_t j=0;j<routes.size();j++)
            if(routes[j].size() >= 1)
            {
                printf("Driver:%d Clients:", (int)j);
                for(size_t k=0;k<routes[j].size();k++)
                    printf("%d ", routes[j][k]);
                printf("\n");
            }

        if(unassigned.size() >= 1)
        {
            printf("Un:");
            for(size_t j=0;j<unassigned.size();j++)
                printf("%d ", unassigned[j]);
            printf("\n");
        }*/

    }


    for (int i = 0; i < assignments.size(); i++) {
        int nbnonzero = 0;
        for (int j = 0; j < assignments[i].size(); j++)
            nbnonzero += assignments[i][j];
        if (nbnonzero == 0) continue;

        Decision *d = reports[0].GetByRequestID(i);
        printf("Req:%2d Assignments:", d->req_id);
        for (int j = 0; j < assignments[i].size(); j++)
            printf("%d ", assignments[i][j]);
        printf("\n");
    }
}

/*
void DecisionMultiSet::ShowArrivals()
{
	printf("DecisionMultiSet::ShowArrivals()\n");
	int max_req_id = -1;
	for(int i=0;i<reports.size();i++)
		for(int k=0;k < reports[i].GetCount(); k++)
		{
			if(!reports[i].Get(k)->is_real) continue;
			max_req_id = std::max(max_req_id, reports[i].Get(k)->req_id);
		}

	std::vector< std::vector<double> > tws(max_req_id+1);
	std::vector< double > tw_starts(max_req_id+1);
	std::vector< double > tw_ends(max_req_id+1);

	for(int i=0;i<=max_req_id;i++)
	{
		for(int j=0;j<reports.size();j++)
			for(int k=0;k<reports[j].GetCount();k++)
			{
				if(reports[j].Get(k)->req_id != i) continue;
				if(reports[j].Get(k)->node_type != NODE_TYPE_DROP) continue;
				if(reports[j].Get(k)->decision_type != DECISION_TYPE_ROUTING) continue;

				tw_starts[i] = reports[j].Get(k)->tw_start_origin;
				tw_ends[i] = reports[j].Get(k)->tw_end_origin;

				if(reports[j].Get(k)->driver_id != -1)
					tws[i].push_back( reports[j].Get(k)->arrival_time );
				else
					tws[i].push_back( -1.0 );
			}
	}

	for(int i=0;i<=max_req_id;i++)
	{
		double min_arr = 999999, max_arr = -99999;
		double avg = 0, var = 0, nb = 0;
		for(size_t j=0;j<tws[i].size();j++)
		{
			if(tws[i][j] < -0.001) continue;
			avg += tws[i][j];
			nb++;
			min_arr = std::min(min_arr, tws[i][j]);
			max_arr = std::max(max_arr, tws[i][j]);
		}
		avg /= nb;
		for(size_t j=0;j<tws[i].size();j++)
		{
			if(tws[i][j] < -0.001) continue;
			var += (avg - tws[i][j])*(avg - tws[i][j]);
		}
		var /= (nb-1);

		printf("Req:%d Tw:%.0lf-%.0lf Nb:%.0lf ", i, tw_starts[i],tw_ends[i],nb);
		printf("AvgArr:%.1lf Std:%.1lf MinMax:%.0lf-%.0lf Arr:",  avg, sqrt(var), min_arr, max_arr);
		for(size_t j=0;j<tws[i].size();j++)
			printf("%.0lf ", tws[i][j]);
		printf("\n");
	}
}

void DecisionMultiSet::GetNextTwDecisions(Decisions & taken_decisions, Decisions & decisions_to_value, Scenarios & sc)
{
	printf("GetNextTwDecisions Time:%.0lf\n", Parameters::GetCurrentTime());
	decisions_to_value.Clear();

	int max_req_id = -1;
	for(int i=0;i<reports.size();i++)
		for(int k=0;k < reports[i].GetCount(); k++)
		{
			if(!reports[i].Get(k)->is_real) continue;
			max_req_id = std::max(max_req_id, reports[i].Get(k)->req_id);
		}

	std::vector< std::vector<double> > tws(max_req_id+1);
	std::vector< double > tw_starts(max_req_id+1);
	std::vector< double > tw_ends(max_req_id+1);
	std::vector< double > tw_start_origins(max_req_id+1);
	std::vector< double > tw_end_origins(max_req_id+1);

	for(int i=0;i<=max_req_id;i++)
	{
		for(int j=0;j<reports.size();j++)
			for(int k=0;k<reports[j].GetCount();k++)
			{
				if(reports[j].Get(k)->req_id != i) continue;
				if(reports[j].Get(k)->node_type != NODE_TYPE_DROP) continue;
				if(reports[j].Get(k)->decision_type != DECISION_TYPE_ROUTING) continue;

				tw_starts[i] = reports[j].Get(k)->tw_start_new;
				tw_ends[i] = reports[j].Get(k)->tw_end_new;
				tw_start_origins[i] = reports[j].Get(k)->tw_start_origin;
				tw_end_origins[i] = reports[j].Get(k)->tw_end_origin;

				double arr = reports[j].Get(k)->arrival_time;
				arr = std::max(arr, tw_starts[i]);
				if(reports[j].Get(k)->driver_id != -1)
					tws[i].push_back( arr );
				else
					tws[i].push_back( -1.0 );
			}
	}


	int best_req = -1; //
	double best_obj = 0, best_avg = 0;
	double best_min = 0, best_max = 0;
	for(int i=0;i<=max_req_id;i++)
	{
		double min_arr = 999999, max_arr = -99999;
		double avg = 0, var = 0, nb = 0;
		for(size_t j=0;j<tws[i].size();j++)
		{
			if(tws[i][j] < -0.001) continue;
			avg += tws[i][j];
			nb++;
			min_arr = std::min(min_arr, tws[i][j]);
			max_arr = std::max(max_arr, tws[i][j]);
		}
		avg /= nb;
		for(size_t j=0;j<tws[i].size();j++)
		{
			if(tws[i][j] < -0.001) continue;
			var += (avg - tws[i][j])*(avg - tws[i][j]);
		}
		var /= (nb-1);

		if(var < 0.001 || max_arr - min_arr < 0.01) continue;

		printf("Req:%d Tw:%.0lf-%.0lf ", i, tw_starts[i],tw_ends[i]);
		printf("OrigTw:%.0lf-%.0lf Nb:%.0lf ", tw_start_origins[i],tw_end_origins[i],nb);
		printf("AvgArr:%.1lf Std:%.1lf MinMax:%.0lf-%.0lf Arr:",  avg, sqrt(var), min_arr, max_arr);
		for(size_t j=0;j<tws[i].size();j++)
			printf("%.0lf ", tws[i][j]);
		printf("\n");

		if(best_obj < var &&  sqrt(var) >= 2)
		{
			best_obj = var;
			best_req = i;
			best_avg = avg;
			best_min = min_arr;
			best_max = max_arr;
		}
	}
	printf("Chosen request:%d obj:%lf Avg:%lf\n", best_req, best_obj, best_avg);
	if(best_req != -1)
	{

		{
			Decision d;
			d.decision_type = DECISION_TYPE_TW_CHANGE;
			d.req_id = best_req;
			d.tw_start_new = tw_starts[best_req];
			//d.tw_end_new = best_avg;
			d.tw_end_new = (best_max + best_min)/2;
			decisions_to_value.Add(d);
		}
		{
			Decision d;
			d.decision_type = DECISION_TYPE_TW_CHANGE;
			d.req_id = best_req;
			d.tw_start_new = (best_max + best_min)/2+1;
			//d.tw_start_new = best_avg;
			d.tw_end_new = tw_ends[best_req];
			decisions_to_value.Add(d);
		}
	}
}*/

void
DecisionMultiSet::GetNextAssignmentDecisions(Decisions &taken_decisions, Decisions &decisions_to_value, Scenarios &sc) {
    //printf("GetNextAssignmentDecisions\n");
    decisions_to_value.Clear();
    int max_req_id = -1;
    for (int i = 0; i < reports.size(); i++)
        for (int k = 0; k < reports[i].GetCount(); k++) {
            if (!reports[i].Get(k)->is_real) continue;
            max_req_id = std::max(max_req_id, reports[i].Get(k)->req_id);
        }

    std::vector<std::vector<int> > assignments(max_req_id + 1);
    for (int i = 0; i < assignments.size(); i++)
        assignments[i].resize(Parameters::GetDriverCount() + 1);

    for (size_t i = 0; i < reports.size(); i++) {
        //printf("\nReport:%zu\n", i);
        //for(int k=0;k < reports[i].GetCount(); k++)
        //	if(reports[i].Get(k)->node_type == NODE_TYPE_DROP)
        //		printf("Client:%d	Driver:%d\n",reports[i].Get(k)->req_id, reports[i].Get(k)->driver_id);

        for (int k = 0; k < reports[i].GetCount(); k++) {
            if (reports[i].Get(k)->node_type != NODE_TYPE_DROP) continue;
            if (!reports[i].Get(k)->is_real) continue;

            assignments[reports[i].Get(k)->req_id][reports[i].Get(k)->driver_id + 1]++;
        }
    }
    //printf("reports\n");
    int best_obj_req = -1; //
    int best_obj = Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_MOST_COMMON ? -1 : 999999;
    int best_nb = 0;
    for (int i = 0; i < assignments.size(); i++) {
        int nbnonzero = 0;
        for (int j = 0; j < assignments[i].size(); j++)
            nbnonzero += assignments[i][j];
        if (nbnonzero == 0) continue;

        //printf("Req:%3d Assignments:", i);
        //for(int j = 0; j < assignments[i].size(); j++)
        //	printf("%2d ", assignments[i][j]);
        //printf("\n");

        int obj = 0;
        int nb = 0;
        for (int j = 0; j < assignments[i].size(); j++)
            if (assignments[i][j] >= 1) {
                obj = std::max(obj, assignments[i][j]);
                nb++;
            }
        if (obj == reports.size()) continue;

        //printf("Req:%3d obj:%3d Assignments:", i, obj);
        //for(int j = 0; j < assignments[i].size(); j++)
        //	printf("%2d ", assignments[i][j]);
        //printf("\tAssDec:%d WasPer:%d\n", (int)taken_decisions.HasAssignmentDecisionReq(i), (int) taken_decisions.HasReqBeenPerformed(i));


        if (((Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_MOST_COMMON && obj > best_obj) ||
             (Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_LEAST_COMMON && obj < best_obj) ||
             (obj == best_obj && nb > best_nb))
            && nb >= 2 && !taken_decisions.HasAssignmentDecisionReq(i)) {
            best_obj = obj;
            best_obj_req = i;
            best_nb = nb;
        }

    }
    //printf("Chosen request:%d obj:%d\n", best_obj_req, best_obj);

    if (best_obj_req != -1) {
        for (int j = 0; j < assignments[best_obj_req].size(); j++)
            if (assignments[best_obj_req][j] >= 1 || Parameters::AssignToAllDrivers() ||
                (j == 0 && Parameters::IncludeUnassignmentDecision())) {
                Decision d;
                d.decision_type = DECISION_TYPE_ASSIGNMENT;
                d.action_type = DECISION_ACTION_ASSIGN_TO;
                d.req_id = best_obj_req;
                d.driver_id = j - 1;
                decisions_to_value.Add(d);
            }
    }
}

void DecisionMultiSet::GetNextAssignmentDecisions2(Decisions &taken_decisions, Decisions &decisions_to_value,
                                                   Scenarios &sc) {
    //printf("GetNextAssignmentDecisions2\n");
    decisions_to_value.Clear();
    int max_req_id = -1;
    for (int i = 0; i < reports.size(); i++)
        for (int k = 0; k < reports[i].GetCount(); k++) {
            if (!reports[i].Get(k)->is_real) continue;
            max_req_id = std::max(max_req_id, reports[i].Get(k)->req_id);
        }

    std::vector<std::vector<int> > assignments(max_req_id + 1);
    for (int i = 0; i < assignments.size(); i++)
        assignments[i].resize(Parameters::GetDriverCount() + 1);

    for (size_t i = 0; i < reports.size(); i++) {
        //printf("\nReport:%zu\n", i);
        //for(int k=0;k < reports[i].GetCount(); k++)
        //	if(reports[i].Get(k)->node_type == NODE_TYPE_DROP)
        //		printf("Client:%d	Driver:%d\n",reports[i].Get(k)->req_id, reports[i].Get(k)->driver_id);

        for (int k = 0; k < reports[i].GetCount(); k++) {
            if (reports[i].Get(k)->node_type != NODE_TYPE_DROP) continue;
            if (!reports[i].Get(k)->is_real) continue;

            assignments[reports[i].Get(k)->req_id][reports[i].Get(k)->driver_id + 1]++;
        }
    }
    //printf("reports\n");
    int best_obj_req = -1; //
    int best_obj = Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_MOST_COMMON ? -1 : 999999;
    int best_nb = 0;
    int best_highest_count = -1;
    int best_highest_id = -1;
    for (int i = 0; i < assignments.size(); i++) {
        int nbnonzero = 0;
        for (int j = 0; j < assignments[i].size(); j++)
            nbnonzero += assignments[i][j];
        if (nbnonzero == 0) continue;

        //printf("Req:%3d Assignments:", i);
        //for(int j = 0; j < assignments[i].size(); j++)
        //	printf("%2d ", assignments[i][j]);
        //printf("\n");

        int obj = 0;
        int nb = 0;
        int highest_id = -1;
        int highest_count = -1;
        for (int j = 0; j < assignments[i].size(); j++)
            if (assignments[i][j] >= 1) {
                obj = std::max(obj, assignments[i][j]);
                if (j >= 1 && highest_count < assignments[i][j]) {
                    highest_count = assignments[i][j];
                    highest_id = j;
                }
                nb++;
            }
        if (obj == reports.size()) continue;

        //printf("Req:%3d obj:%3d Assignments:", i, obj);
        //for(int j = 0; j < assignments[i].size(); j++)
        //	printf("%2d ", assignments[i][j]);
        //printf("\tAssDec:%d\n", (int)taken_decisions.HasAssignmentDecisionReq(i, DECISION_ACTION_ASSIGN_TO, highest_id-1));


        if (((Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_MOST_COMMON && obj > best_obj) ||
             (Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_LEAST_COMMON && obj < best_obj) ||
             (obj == best_obj && nb > best_nb))
            && nb >= 2 &&
            !taken_decisions.HasAssignmentDecisionReq(i, DECISION_ACTION_ASSIGN_TO, highest_id - 1)) {
            best_obj = obj;
            best_obj_req = i;
            best_nb = nb;
            best_highest_count = highest_count;
            best_highest_id = highest_id;
        }

    }
    //printf("Chosen request:%d obj:%d count:%d driver:%d\n", best_obj_req, best_obj, best_highest_count, best_highest_id);

    if (best_obj_req != -1) {
        Decision d;
        d.decision_type = DECISION_TYPE_ASSIGNMENT;
        d.action_type = DECISION_ACTION_ASSIGN_TO;
        d.req_id = best_obj_req;
        d.driver_id = best_highest_id - 1;
        decisions_to_value.Add(d);

        d.action_type = DECISION_ACTION_DONT_ASSIGN_TO;
        d.req_id = best_obj_req;
        d.driver_id = best_highest_id - 1;
        decisions_to_value.Add(d);

        if (Parameters::IncludeUnassignmentDecision() || assignments[best_obj_req][0] >= 1) {
            d.action_type = DECISION_ACTION_ASSIGN_TO;
            d.req_id = best_obj_req;
            d.driver_id = -1;
            decisions_to_value.Add(d);
        }
    }
}

void
DecisionMultiSet::GetNextActionDecisions(Decisions &taken_decisions, Decisions &decisions_to_value, Scenarios &sc) {
    //printf("GetNextActionDecisions\n");
    decisions_to_value.Clear();
    int max_req_id = -1;
    for (int i = 0; i < reports.size(); i++)
        for (int k = 0; k < reports[i].GetCount(); k++) {
            if (!reports[i].Get(k)->is_real) continue;
            max_req_id = std::max(max_req_id, reports[i].Get(k)->req_id);
        }


    std::vector<std::vector<int> > assignments(max_req_id + 1);
    for (int i = 0; i < assignments.size(); i++)
        assignments[i].resize(Parameters::GetDriverCount() + 1);

    std::vector<int> go_now_requests(max_req_id + 1, 0);
    std::vector<int> wait_requests(max_req_id + 1, 0);
    std::vector<int> dont_deliver_requests(max_req_id + 1, 0);

    //reports[0].Show();
    //for(size_t i=0;i<reports.size();i++)
    {
        //	printf("\n\n\nReport:%d\n", (int)i);
        //	reports[i].ShowOnlyRealCustomer();
    }

    for (size_t i = 0; i < reports.size(); i++) {
        for (int j = 0; j < reports[i].GetCount(); j++) {
            Decision *d = reports[i].Get(j);

            if (!d->is_real) continue;
            if (d->decision_type != DECISION_TYPE_ROUTING) continue;
            if (d->departure_time < Parameters::GetCurrentTime() && d->driver_id != -1) continue;
            if (d->node_type != NODE_TYPE_PICKUP) continue;
            if (d->driver_id == -1 && !d->is_still_feasible) continue;
            //printf("Report:%d ", (int)i);d->Show();

            if (d->node_type == NODE_TYPE_PICKUP && d->arrival_time == Parameters::GetCurrentTime())
                go_now_requests[d->req_id]++;

            if (d->node_type == NODE_TYPE_PICKUP && d->arrival_time > Parameters::GetCurrentTime())
                wait_requests[d->req_id]++;

            if (d->node_type == NODE_TYPE_PICKUP && d->driver_id == -1)
                dont_deliver_requests[d->req_id]++;
        }
    }

    //for(int i=0;i<=max_req_id;i++)
    //	if(go_now_requests[i]+ wait_requests[i]+ dont_deliver_requests[i] >= 1)
    //		printf("Req:%d GoNow:%d Wait:%d Dont:%d\n", i, go_now_requests[i], wait_requests[i], dont_deliver_requests[i]);

    double sum_go_now = 0, sum_wait = 0, sum_dont = 0;
    for (int i = 0; i <= max_req_id; i++) {
        sum_go_now += go_now_requests[i];
        sum_wait += wait_requests[i];
        sum_dont += dont_deliver_requests[i];
    }
    int cnt = sum_go_now + sum_wait + sum_dont;
    sum_go_now /= cnt;
    sum_wait /= cnt;
    sum_dont /= cnt;
    //printf("GoNow:%.3lf Wait:%.3lf Dont:%.3lf\n",sum_go_now, sum_wait,sum_dont);
    //if(sum_wait > 0.9)
    //	return;

    int best_req = -1;
    int best_obj = (int) reports.size();
    if (Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_MOST_COMMON)
        best_obj = -1;

    for (int i = 0; i <= max_req_id; i++) {
        if (taken_decisions.HasActionDecisionReqExceptDontUnassign(i)) continue;

        //int max_v = std::max(go_now_requests[i], std::max(wait_requests[i], dont_deliver_requests[i]));
        int max_v = go_now_requests[i];
        //printf("req:%d best_obj:%d max_v:%d\n",i, best_obj, max_v);

        if (go_now_requests[i] >= 1 && max_v < best_obj && max_v > 0 &&
            Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_LEAST_COMMON)
            //if(max_v < best_obj && max_v > 0 && Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_LEAST_COMMON)
        {
            best_req = i;
            best_obj = max_v;
        }

        if (go_now_requests[i] >= 1 && max_v > best_obj && max_v < reports.size() && max_v > 0 &&
            Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_MOST_COMMON)
            //if(max_v > best_obj && max_v < reports.size() && max_v > 0 && Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_MOST_COMMON)
        {
            best_req = i;
            best_obj = max_v;
        }
    }

    int sum = 0;
    if (best_req != -1)
        sum = go_now_requests[best_req] + wait_requests[best_req] + dont_deliver_requests[best_req];
    //printf("Chosen request:%d obj:%d sum:%d\t Avg Cost:%.2lf\n", best_req, best_obj, sum, GetAverageCost());


    if (best_req != -1) {
        Decision d;
        d.req_id = best_req;
        d.decision_type = DECISION_TYPE_ACTION;

        d.action_type = DECISION_ACTION_GO_NOW;
        decisions_to_value.Add(d);
        d.action_type = DECISION_ACTION_WAIT;
        decisions_to_value.Add(d);

        if (!Parameters::PerformOrderAcceptancy() &&
            (!Parameters::EvaluateRejectOnlyIfNot0() || dont_deliver_requests[best_req] >= 1)) {
            d.decision_type = DECISION_TYPE_ASSIGNMENT;
            d.driver_id = -1;
            decisions_to_value.Add(d);
        }
    }
    //exit(1);
}

void
DecisionMultiSet::GetMajorityActionDecisions(Decisions &taken_decisions, Decisions &decisions_to_value, Scenarios &sc) {
    printf("GetMajorityActionDecisions\n");
    decisions_to_value.Clear();
    int max_req_id = -1;
    for (int i = 0; i < reports.size(); i++)
        for (int k = 0; k < reports[i].GetCount(); k++) {
            if (!reports[i].Get(k)->is_real) continue;
            max_req_id = std::max(max_req_id, reports[i].Get(k)->req_id);
        }


    std::vector<std::vector<int> > assignments(max_req_id + 1);
    for (int i = 0; i < assignments.size(); i++)
        assignments[i].resize(Parameters::GetDriverCount() + 1);

    std::vector<int> go_now_requests(max_req_id + 1, 0);
    std::vector<int> wait_requests(max_req_id + 1, 0);
    std::vector<int> dont_deliver_requests(max_req_id + 1, 0);

    for (size_t i = 0; i < reports.size(); i++) {
        for (int j = 0; j < reports[i].GetCount(); j++) {
            Decision *d = reports[i].Get(j);

            if (!d->is_real) continue;
            if (d->decision_type != DECISION_TYPE_ROUTING) continue;
            if (d->departure_time < Parameters::GetCurrentTime() && d->driver_id != -1) continue;
            if (d->node_type != NODE_TYPE_PICKUP) continue;

            if (d->node_type == NODE_TYPE_PICKUP && d->arrival_time == Parameters::GetCurrentTime())
                go_now_requests[d->req_id]++;

            if (d->node_type == NODE_TYPE_PICKUP && d->arrival_time > Parameters::GetCurrentTime())
                wait_requests[d->req_id]++;

            if (d->node_type == NODE_TYPE_PICKUP && d->driver_id == -1)
                dont_deliver_requests[d->req_id]++;
        }
    }

    for (int i = 0; i <= max_req_id; i++)
        if (go_now_requests[i] + wait_requests[i] + dont_deliver_requests[i] >= 1)
            printf("Req:%d GoNow:%d Wait:%d Dont:%d\n", i, go_now_requests[i], wait_requests[i],
                   dont_deliver_requests[i]);

    for (int i = 0; i <= max_req_id; i++)
        if (go_now_requests[i] + wait_requests[i] + dont_deliver_requests[i] >= 1) {
            if (go_now_requests[i] >= wait_requests[i] && go_now_requests[i] >= dont_deliver_requests[i]) {
                Decision d;
                d.req_id = i;
                d.decision_type = DECISION_TYPE_ACTION;
                d.action_type = DECISION_ACTION_GO_NOW;
                decisions_to_value.Add(d);
            } else if (wait_requests[i] >= go_now_requests[i] && wait_requests[i] >= dont_deliver_requests[i]) {
                Decision d;
                d.req_id = i;
                d.decision_type = DECISION_TYPE_ACTION;
                d.action_type = DECISION_ACTION_WAIT;
                decisions_to_value.Add(d);
            } else {
                Decision d;
                d.req_id = i;
                d.decision_type = DECISION_TYPE_ASSIGNMENT;
                d.driver_id = -1;
                decisions_to_value.Add(d);
            }

        }
}

void DecisionMultiSet::ShowOnlyRealCustomer() {
    for (size_t i = 0; i < reports.size(); i++) {
        printf("Report:%d\n", (int) i);
        reports[i].ShowOnlyRealCustomer();
    }
}

void DecisionMultiSet::ShowOnlyForRequest(int req_id) {
    for (size_t i = 0; i < reports.size(); i++) {
        printf("Report:%d\n", (int) i);
        reports[i].ShowOnlyForRequest(req_id);
    }
}

void DecisionMultiSet::ShowOnlyForDriver(int driver_id) {
    for (size_t i = 0; i < reports.size(); i++) {
        printf("Report:%d\n", (int) i);
        reports[i].ShowByDriver(driver_id);
    }
}

bool DecisionMultiSet::GetNextDecisionProgressive(Decisions &decisions, Decision &decision) {
    int max_req_id = -1;
    for (auto &report: reports)
        for (int k = 0; k < report.GetCount(); k++) {
            if (!report.Get(k)->is_real) continue;
            max_req_id = std::max(max_req_id, report.Get(k)->req_id);
        }

    std::vector<int> go_now_requests(max_req_id + 1, 0);
    std::vector<int> wait_requests(max_req_id + 1, 0);
    std::vector<int> dont_deliver_requests(max_req_id + 1, 0);

    for (auto &report: reports)
        for (int j = 0; j < report.GetCount(); j++) {
            Decision *d = report.Get(j);

            if (!d->is_real) continue;
            if (d->decision_type != DECISION_TYPE_ROUTING) continue;
            if (d->departure_time < Parameters::GetCurrentTime() && d->driver_id != -1) continue;
            if (d->node_type != NODE_TYPE_PICKUP) continue;
            if (d->driver_id == -1 && !d->is_still_feasible) continue;

            if (d->node_type == NODE_TYPE_PICKUP && d->arrival_time == Parameters::GetCurrentTime())
                go_now_requests[d->req_id]++;

            if (d->node_type == NODE_TYPE_PICKUP && d->arrival_time > Parameters::GetCurrentTime())
                wait_requests[d->req_id]++;

            if (d->node_type == NODE_TYPE_PICKUP && d->driver_id == -1)
                dont_deliver_requests[d->req_id]++;
        }

    int best_req = -1;
    int best_type = -1;
    int best_obj = (int) reports.size();
    if (Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_MOST_COMMON)
        best_obj = -1;

    for (int i = 0; i <= max_req_id; i++) {
        if (decisions.HasActionDecisionReqExceptDontUnassign(i)) continue;

        int max_v = std::max(go_now_requests[i], std::max(wait_requests[i], dont_deliver_requests[i]));
        // int max_v = go_now_requests[i];

        int type = -1;

        if (go_now_requests[i] == max_v)
            type = DECISION_ACTION_GO_NOW;
        else if (wait_requests[i] == max_v)
            type = DECISION_ACTION_WAIT;
        else if (dont_deliver_requests[i] == max_v)
            type = DECISION_ACTION_DONT_DELIVER;

        if (go_now_requests[i] > 0) {
            type = DECISION_ACTION_GO_NOW;
        } else if (wait_requests[i] > 0) {
            type = DECISION_ACTION_WAIT;
        } else if (dont_deliver_requests[i] > 0) {
            type = DECISION_ACTION_DONT_DELIVER;
        }

        if (type >= DECISION_ACTION_GO_NOW && max_v < best_obj && max_v > 0 &&
            Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_LEAST_COMMON) {
            best_req = i;
            best_type = type;
        }

        if (type >= DECISION_ACTION_GO_NOW && max_v > best_obj && max_v < reports.size() && max_v > 0 &&
            Parameters::GetDecisionSelectionMode() == DECISION_SELECTION_MODE_MOST_COMMON) {
            best_req = i;
            best_type = type;
        }
    }

    if (best_req != -1) {
        decision.req_id = best_req;
        decision.decision_type = DECISION_TYPE_ACTION;

        decision.action_type = best_type;

        if (!Parameters::PerformOrderAcceptancy() &&
            (!Parameters::EvaluateRejectOnlyIfNot0() || dont_deliver_requests[best_req] >= 1)) {
            decision.decision_type = DECISION_TYPE_ASSIGNMENT;
            decision.driver_id = -1;
        }
        return true;
    }

    return false;
}
