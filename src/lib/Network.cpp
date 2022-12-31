
#include "Network.h"


Network::Network(int nbnodes): nodes(nbnodes)
{

}
Network::~Network()
{
	for(size_t i = 0;i<arcs.size();i++)
		delete arcs[i];
	arcs.clear();
	nodes.clear();
	map_arcs.clear();
}

//add an Arc
void Network::AddArc(int from, int to)
{
	AddArc(from, to, (int)arcs.size());
}
void Network::AddArc(int from, int to, int id)
{
	network_arc_ptr arc = new network_arc_t();
	arc->id = (int)arcs.size();
	arc->from = from;
	arc->to = to;
	arc->id = id;
	arcs.push_back(arc);
	nodes[from].arcs.push_back(arc);
	map_arcs[id] = arc;
	//printf("from:%d to:%d id:%d\n", from,to,id);
}

//set the basic information of an arc
void Network::SetArc(int id, char can_visit, double value)
{
	network_arc_ptr arc = map_arcs[id];
	arc->can_visit = can_visit;
	arc->value = value;
}
void Network::SetArc(int id, char can_visit, double value, double cost)
{
	network_arc_ptr arc = map_arcs[id];
	arc->can_visit = can_visit;
	arc->value = value;
	arc->cost = cost;
}

//set the basic information of an arc
void Network::SetArc(int id, char can_visit)
{
	network_arc_ptr arc = map_arcs[id];
	arc->can_visit = can_visit;
}

void Network::CloseOutGoingArcs(int from)
{
	network_node_ptr node = &nodes[from];
	for(int i=0;i<node->GetArcCount();i++)
		node->GetArc(i)->can_visit = 0;
}

network_arc_ptr Network::GetArc(int id)
{
	return map_arcs[id];
}

network_node_ptr Network::GetNode(int from)
{
	return &nodes[from];
}

//run a Breadth-First Search from an origin node and returns the number of marked arcs
void Network::BFS(int from)
{
	network_bfs_report_t report;
	BFS(from, &report);
}
void Network::BFS(int from, network_bfs_report_ptr report)
{
	for(size_t i = 0;i<arcs.size();i++)
		arcs[i]->init();
	for(size_t i = 0;i<nodes.size();i++)
		nodes[i].init();

	report->init();
	report->nbnodes = (int)nodes.size();
	report->nbarcs = (int)arcs.size();
	std::vector<network_node_ptr> to_visit;
	to_visit.push_back( &nodes[from] );

	int nb_visited = 0;
	while(nb_visited < (int)to_visit.size())
	{
		network_node_ptr node = to_visit[nb_visited];
		node->visited = 1;
		node->marked = 1;

		for(int i=0;i<node->GetArcCount();i++)
		{
			network_arc_ptr arc = node->GetArc(i);
			if(arc->can_visit == 0) continue;

			arc->marked = 1;

			if(nodes[ arc->to ].marked == 0)
			{
				nodes[ arc->to ].marked = 1;
				to_visit.push_back( &nodes[ arc->to ]);
			}
		}
		nb_visited++;
	}
	//printf("nbvisited:%d\n", nb_visited);
	for(size_t i = 0;i<arcs.size();i++)
	{
		report->nb_marked_arcs += arcs[i]->marked;
		if(arcs[i]->value > 0.0001) report->nb_positive_arcs++;
		if(arcs[i]->value > 0.0001 && arcs[i]->can_visit == 1)
			report->nb_marked_positive_arcs+= arcs[i]->marked;
	}

	for(size_t i = 0;i<nodes.size();i++)
		report->nb_marked_nodes += nodes[i].marked;
}

bool Network::IsVisited(int node)
{
	return nodes[node].visited == 1;
}

void Network::CalculateOutArcs()
{
	for(size_t i = 0;i<nodes.size();i++)
		nodes[i].out_arcs_count = 0;
	for(size_t i = 0;i<nodes.size();i++)
	{
		network_node_ptr node = &nodes[i];
		for(int j=0;j<node->GetArcCount();j++)
		{
			network_arc_ptr arc = node->GetArc(j);
			nodes[arc->to].out_arcs_count++;
		}
	}
}

void Network::PrintGraphViz(char * filename)
{
	CalculateOutArcs();
	FILE * f = fopen(filename, "w");
	if(f == NULL) return;

	fprintf(f,"digraph G {\n");
 	fprintf(f,"size = \"10, 10\";\n");
 	fprintf(f,"overlap = scale;\n");
 	fprintf(f,"splines = true;\n");
 	for(size_t i = 0;i<nodes.size();i++)
	{
		if(nodes[i].GetArcCount() >= 1 || nodes[i].out_arcs_count >= 1)
			fprintf(f,"%d [pos = \"%d,%d!\"]\n", (int)i,0,(int)i);
	}
	for(size_t i = 0;i<nodes.size();i++)
	{
		network_node_ptr node = &nodes[i];
		for(int j=0;j<node->GetArcCount();j++)
		{
			network_arc_ptr arc = node->GetArc(j);
			fprintf(f,"%d -> %d[label=\"%.2lf x %d\"];\n", arc->from,arc->to,arc->value, (int) arc->cost);
		}
	}


	/*
	for(size_t i = 0;i+1<nodes.size();i++)
		for(size_t j = i+1;j<nodes.size();j++)
			if( (nodes[i].GetArcCount() >= 1 || nodes[i].out_arcs_count >= 1) &&
				(nodes[j].GetArcCount() >= 1 || nodes[j].out_arcs_count >= 1))
				{
					fprintf(f,"%d -> %d [style=invis];\n", (int)i,(int)j);
					break;
				}
	*/
	fprintf(f,"}\n");
	fclose(f);
}
