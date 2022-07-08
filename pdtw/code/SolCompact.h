/*
 * Copyright Jean-Francois Cote 2021
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
 *
 *
*/

#ifndef SOLUTIONCOMPACT
#define SOLUTIONCOMPACT

#include <vector>
#include <string>

template <class NodeT, class DriverT>
class SolCompact
{
	public :
		std::string name;
		double distance;
		double cost;
		std::vector< std::vector<NodeT> > routes;
		std::vector<NodeT> unassigneds;

		void GetNodes(std::vector<NodeT> & nodes)
		{
			nodes.clear();
			for(size_t i=0;i<routes.size();i++)
				for(size_t j=0;j<routes[i].size();j++)
					nodes.push_back(routes[i][j]);
			for(size_t i=0;i<unassigneds.size();i++)
				nodes.push_back(unassigneds[i]);
		}

		void Show()
		{
			printf("Solution cost:%.1lf un:%d dist:%.1lf\n", cost, (int)unassigneds.size(), distance);
			for(size_t i=0;i<routes.size();i++)
			{
				printf("Route:%d nodes:", (int)i);
				for(size_t j=0;j<routes[i].size();j++)
				{
					if(routes[i][j].type == NODE_TYPE_START_DEPOT)
						printf("Depot:%.1lf ", routes[i][j].departure_time);
					else if(routes[i][j].type == NODE_TYPE_END_DEPOT)
						printf("Depot:%.1lf ", routes[i][j].arrival_time);
					else
					{
						if(routes[i][j].type == NODE_TYPE_PICKUP)
							printf("(%dp ", routes[i][j].id);
						else if(routes[i][j].type == NODE_TYPE_DROP)
							printf("(%dd ", routes[i][j].id);
						else
							printf("(%dx ", routes[i][j].id);
						printf("%d-%d arr:%.1lf) ", (int)routes[i][j].tw_start, (int)routes[i][j].tw_end, routes[i][j].arrival_time);
					}
				}
				printf("\n");
			}
			printf("Un(%d):", (int)unassigneds.size());
			for(size_t j=0;j<unassigneds.size();j++)
				printf("%d-", unassigneds[j].id);
			printf("\n");
		}
};

#endif
