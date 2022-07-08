
/*
 * Copyright Jean-Francois Cote 2020
 *
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
 *
 *
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>



#include "Scenarios.h"
#include "Solver.h"
#include "BranchRegretSimulation.h"
#include "DynSimulation.h"
#include "StaticSimulation.h"
#include "ConsensusSimulation.h"
#include <vector>
#include "../SolCompact.h"
#include "Test.h"


void Execute(Scenarios & scenarios, config & conf, std::vector<SolCompact<Node,Driver>> & sols);
void PrintSolutionLatex(char * filename, Scenarios & scs, std::vector<SolCompact<Node,Driver>> & sols);

int main(int arg, char ** argv)
{
	time_t tt = time(0);
	tt = 15646022;
	srand(tt);
	printf("Seed:%ld\n", tt);
	//srand(1304445903);

	Scenarios scenarios;
	if(arg >= 3)
		scenarios.LoadStacyVoccia(argv[2]);
	else
	{
		printf("Provide the number of drivers\n");
		printf("Provide the config file\n");
		exit(1);
	}

	int nb_drivers = 1;
	sscanf(argv[1], "%d", &nb_drivers);

	std::vector<SolCompact<Node,Driver>> sols;
	std::vector<config> configs;
	configs.push_back(config(100,nb_drivers,0,30));


	printf("File:%s\nInstance:%s Customers:%d\n", argv[2], scenarios.problem_name_tw.c_str(), scenarios.nb_real_requests_instance);

	for(size_t i=0;i<configs.size();i++)
		Execute(scenarios, configs[i], sols);


	for(size_t i=0;i<configs.size();i++)
		configs[i].Show();


	if(arg >= 3)
	{
		FILE * f = fopen(argv[3], "w");
		if(f != NULL)
		{
			fprintf(f,"%s;%s;%s;%d;", argv[2], scenarios.problem_name_tw.c_str(),scenarios.actual_scenario_id.c_str(),scenarios.nb_real_requests_instance);
			for(size_t i=0;i<configs.size();i++)
				configs[i].Print(f);
			fprintf(f,"\n");
			fclose(f);
		}
	}

	//if(arg >= 4)
	//	PrintSolutionLatex( argv[4], scenarios, sols);

	return 0;
}


void Execute(Scenarios & scenarios, config & conf, std::vector<SolCompact<Node,Driver>> & sols)
{
	Parameters::SetAlnsIterations(conf.alns);
	Parameters::SetDriverCount(conf.driver_count);
	Parameters::SetScenarioCount(conf.scenario_count);
	Parameters::SetP(conf.time_horizon);

	Parameters::SetGenerateIntelligentScenario(true);
	Parameters::SetOrderAcceptancy(false);
	Parameters::SetEvaluateWaitingStrategy(true);

	int c = scenarios.nb_real_requests_instance;

	printf("\n\nExecute AlnsIterations:%d Drivers:%d ", Parameters::GetAlnsIterations(), Parameters::GetDriverCount());
	printf("P:%d Scenarios:%d \n", Parameters::GetP(), Parameters::GetScenarioCount());
	{
	StaticSimulation sim;
	sim.Optimize(scenarios);
	conf.results.push_back( result(1,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	sols.push_back( sim.final_solution );
}
	{
	DynSimulation sim;
	Parameters::SetAllowEnRouteDepotReturns(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(2,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}

	{
	DynSimulation sim;
	Parameters::SetAllowEnRouteDepotReturns(true);
	sim.Optimize(scenarios);
	conf.results.push_back( result(3,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}

	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_STACY );
	Parameters::SetAllowEnRouteDepotReturns(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(4,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}


	{
	ConsensusSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	Parameters::SetAllowEnRouteDepotReturns(false);
	sim.Optimize(scenarios);
	conf.results.push_back( result(7,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	}


	{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	Parameters::SetAllowEnRouteDepotReturns(false);
	Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
	sim.Optimize(scenarios);
	conf.results.push_back( result(9,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	conf.results.back().nb_branchings = sim.nb_branchings;
	conf.results.back().nb_evaluated_nodes = sim.nb_evaluated_nodes;
	}

	{
	BranchRegretSimulation sim;
	Parameters::SetConsensusToUse( CONSENSUS_BY_DRIVERS );
	Parameters::SetAllowEnRouteDepotReturns(true);
	Parameters::SetDecisionSelectionMode(DECISION_SELECTION_MODE_MOST_COMMON);
	sim.Optimize(scenarios);
	conf.results.push_back( result(9,c,sim.distance,sim.unassigneds, sim.time_taken,sim.nb_routes, sim.nb_events) );
	conf.results.back().nb_branchings = sim.nb_branchings;
	conf.results.back().nb_evaluated_nodes = sim.nb_evaluated_nodes;
	}
}


void PrintSolutionLatex(char * filename, Scenarios & scs, std::vector<SolCompact<Node,Driver>> & sols)
{
	std::vector<std::string> colors;
	colors.push_back("white");
	colors.push_back("yellow");
	colors.push_back("cyan");
	colors.push_back("lightgray");
	colors.push_back("lime");
	colors.push_back("blue");
	colors.push_back("brown");
	colors.push_back("red");
	colors.push_back("pink");
	colors.push_back("gray");
	colors.push_back("green");
	colors.push_back("magenta");
	colors.push_back("olive");
	colors.push_back("orange");
	colors.push_back("purple");
	colors.push_back("teal");
	colors.push_back("violet");
	colors.push_back("darkgray");
	colors.push_back("black");

	char temp[1000];
	FILE * f = fopen(filename, "w");
	fprintf(f, "\\documentclass[11pt]{article}\n");
	fprintf(f, "\\usepackage[textwidth=18.0cm, textheight=23.0cm, top=2.0cm]{geometry}\n");
	fprintf(f, "\\usepackage{pst-all}\n");
	fprintf(f, "\\usepackage{amssymb}\n");
	fprintf(f, "\\usepackage{tikz}\n");
	fprintf(f, "\\begin{document}");
	fprintf(f, "\\pagestyle{empty}\n" );


	double minx = scs.GetDepot()->x, maxx = scs.GetDepot()->x, miny = scs.GetDepot()->y, maxy = scs.GetDepot()->y;
	for(int i=0;i<scs.GetScenarioCount();i++)
		for(int j=0;j<scs.GetScenario(i)->GetNodeCount();j++)
		{
			Node * n = scs.GetScenario(i)->GetNode(j);
			minx = std::min(minx, n->x);
			miny = std::min(miny, n->y);
			maxx = std::max(maxx, n->x);
			maxy = std::max(maxy, n->y);
		}
	for(int j=0;j<scs.GetRealScenarioCustomerCount();j++)
	{
		Node * n = scs.GetRealScenarioCustomer(j);
		minx = std::min(minx, n->x);
		miny = std::min(miny, n->y);
		maxx = std::max(maxx, n->x);
		maxy = std::max(maxy, n->y);
	}





	for(size_t i = 0; i < sols.size(); i++)
	{
		if(i >= 1) fprintf(f, "\\newpage\n");
		fprintf(f, "\\begin{figure}\n");
		fprintf(f, "\\centering\n");
		fprintf(f, "\\begin{tikzpicture}[shorten >=1pt,scale=1.0,every node/.style={scale=1.0},->]");
		fprintf(f, "\\tikzstyle{vertex}=[circle,fill=black!25,minimum size=14pt,inner sep=0pt]\n");

		SolCompact<Node,Driver> & sol = sols[i];
		std::vector<Node> nodes;
		sol.GetNodes(nodes);

		fprintf(f, "\\foreach \\name/\\x/\\y in {");
		for(size_t j = 0; j < nodes.size(); j++)
		{
			Node * n = &nodes[j];
			double newx = 10 * (n->x - minx) / (maxx - minx);
			double newy = 10 * (n->y - miny) / (maxy - miny);
			fprintf(f, " %d/%lf/%lf", n->origin_id, newy, newx);
			if(j+1 < nodes.size())
				fprintf(f, ",");
		}
		fprintf(f,"}\n");
		fprintf(f, "\\node[circle,draw, inner sep = 1pt] [fill = white] (G-\\name) at (\\x,\\y){\\name} ;\n");

		double newx = 10 * (scs.GetDepot()->x - minx) / (maxx - minx);
		double newy = 10 * (scs.GetDepot()->y - miny) / (maxy - miny);
		fprintf(f, "\\node[rectangle,draw,minimum size=0.1cm, inner sep = 1pt] [fill = black] (G-%d) at (%lf,%lf){} ;\n", 0, newy, newx);
		for(size_t k = 0; k < sol.routes.size(); k++)
		{
			if(sol.routes[k].size() <= 2) continue;

			fprintf(f, "\\foreach \\from/\\to in {");
			for(size_t l=0;l+1<sol.routes[k].size();l++)
			{
				Node * prev = &sol.routes[k][l];
				Node * next = &sol.routes[k][l+1];
				int from = prev->IsCustomer()?prev->origin_id:0;
				int to = next->IsCustomer()?next->origin_id:0;
				fprintf(f,  "%d/%d", from, to);

				if(next->type != NODE_TYPE_END_DEPOT)
					fprintf(f, ",");
			}
			fprintf(f, "}\n");
			fprintf(f, "\\draw (G-\\from) -- (G-\\to) node [midway,above,sloped](G-\\from-\\to){};\n");
			//fprintf(f, "\\end{center}\n");
		}

		int un = sol.unassigneds.size()/2;
		fprintf(f, "\\node[above, font = \\large\\bfseries] at(current bounding box.north) {%s Un:%d Dist:%.1lf};\n", sol.name.c_str(), un, sol.distance);

		/*fprintf(f, "\\matrix[draw, below left] at(current bounding box.south west) {\n");
		fprintf(f, "\\node[label=right:Schedule]{}; \\\\ \n");
		for(int j=0;j<zonelist->GetZoneCount();j++)
		{
			Zone * z = zonelist->GetZone(j); z->Sort();
			fprintf(f, "\\node[circle, draw, inner sep = 3pt, fill = %s,label = right:zone:%d tws:",colors[j].c_str(), z->GetId());
			for(int k=0;k<z->GetTimeWindowCount();k++)
			{
				TimeWindow * tw = z->GetTimeWindow(k);
				fprintf(f,"(day:%d %d-%d) ", (int)tw->day, tw->GetStartTime()/60, tw->GetEndTime()/60);
			}
			fprintf(f, "]{}; \\\\ \n");
		}
		fprintf(f, "\\node[rectangle,draw,inner sep = 3pt, fill = black, label = right:depot]{}; \\\\ \n");
		fprintf(f, "\\node[circle,draw,inner sep = 1pt, label = right:Total cust:%.1lf dist:%.1lf]{}; \\\\ \n",tc, td);
		fprintf(f, "};\n");*/



		fprintf(f, "\\end{tikzpicture}\n");
		fprintf(f, "\\end{figure}\n");
		if(i+1 < sols.size())
			fprintf(f, "\\newpage\n");
	}
	fprintf(f, "\\end{document}");
	fclose(f);
}
