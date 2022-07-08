/*
 * Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
*/

#ifndef WORST_REQUEST_REMOVAL_H
#define WORST_REQUEST_REMOVAL_H


#include "RemoveOperator.h"
#include "ProblemDefinition.h"
#include "Solution.h"
#include "constants.h"
#include "Request.h"
#include <vector>
#include <algorithm>
#include "../../lib/mathfunc.h"

template< class NodeT>
struct WorstRequestRemovalItem
{
	Request<NodeT> * r;
	double cost;
	WorstRequestRemovalItem(Request<NodeT> * r1, double cost1) : r(r1), cost(cost1){}
	bool operator < (const WorstRequestRemovalItem<NodeT> * it) const
	{
		return (cost > it->cost);
	}
	bool operator < (const WorstRequestRemovalItem<NodeT> & it) const
	{
		return (cost > it.cost);
	}
};

template <class NodeT, class DriverT>
class IWorstRequestRemovalCost
{
public:
	virtual double GetCost(Sol<NodeT, DriverT> & s, Request<NodeT> * r) = 0;
};

template <class NodeT, class DriverT>
class WorstRequestRemovalCostNode : public IWorstRequestRemovalCost<NodeT, DriverT>
{
	public:
	double GetCost(Sol<NodeT, DriverT> & s, Request<NodeT> * r)
	{
		double ** m = s.GetDistances();
		double worst_cost = 0;
		for(int i=0;i<r->GetNodeCount();i++)
		{
			NodeT * n = r->GetNode(i);
			NodeT * prev = s.Prev[n->id];
			NodeT * next = s.Next[n->id];
			double cc = m[prev->distID][n->distID] + m[n->distID][next->distID] - m[prev->distID][next->distID];
			worst_cost = std::max( cc, worst_cost);
		}
		return worst_cost;
	}

	private:
		double ** _distances;
};

template <class NodeT, class DriverT>
class WorstRequestRemovalCostRequest : public IWorstRequestRemovalCost<NodeT, DriverT>
{
	public:
	double GetCost(Sol<NodeT, DriverT> & s, Request<NodeT> * r)
	{
		double ** m = s.GetDistances();
		double req_arc_cost = 0;
		double no_req_arc_cost = 0;
		NodeT * prev = s.GetNode( s.GetAssignedTo(r->GetParent())->StartNodeID );
		NodeT * prev_not_r = prev;
		while(prev->type != NODE_TYPE_END_DEPOT)
		{
			NodeT * next = s.Next[prev->id];

			if(prev->Num_Request == r->id && next->Num_Request != r->id)
				no_req_arc_cost += m[prev_not_r->distID][next->distID];

			if(prev->Num_Request == r->id || next->Num_Request == r->id)
				req_arc_cost += m[prev->distID][next->distID];

			if(next->Num_Request != r->id)
				prev_not_r = next;
			prev = next;
		}

		return req_arc_cost - no_req_arc_cost;
	}

	private:
		double ** _distances;
};


template <class NodeT, class DriverT>
class WorstRequestRemoval : public RemoveOperator<NodeT,DriverT>
{
	public:
		WorstRequestRemoval()
		{
			removal_cost = &removal_cost_node;
		}
		~WorstRequestRemoval(){}
		void SetToRequestCostRemoval(){removal_cost = &removal_cost_request;}
		void SetToNodeCostRemoval(){removal_cost = &removal_cost_node;}

		void Remove(Sol<NodeT, DriverT> & s, int count)
		{
			Fill(s);

			for(int l=0;l<items.size();l++)
				items[l].cost = removal_cost->GetCost(s,items[l].r);

			while(items.size() >= 1 && count-- >= 0)
			{
				sort(items.begin(), items.end());
				//Show();

				int index = ((int)items.size()) * pow(mat_func_get_rand_double(), ALNS_P);
				//printf("index:%d mv_cost:%.2lf cost:%.2lf\n", index, items[index].cost,s.GetCost());

				int driver_id = s.GetAssignedTo(items[index].r->GetParent())->id;
				s.RemoveAndUnassign(items[index].r);
				items[index] = items[items.size() - 1];
				items.pop_back();

				for(int l=0;l<items.size();l++)
					if(s.GetAssignedTo(items[l].r->GetParent())->id == driver_id)
						items[l].cost = removal_cost->GetCost(s,items[l].r);
			}
			s.MakeFeasible();
		}

		void Fill(Sol<NodeT, DriverT> & s)
		{
			items.clear();
			for(int i=0;i<s.GetRequestCount();i++)
			{
				Request<NodeT>* r = s.GetRequest(i);
				if (s.GetAssignedTo(r->GetParent()) != NULL)
					items.push_back( WorstRequestRemovalItem<NodeT>(r,0) );
			}
		}


		void Show()
		{
			printf("WorstRequestRemoval::Show()\n");
			for(size_t i=0;i<items.size();i++)
				printf("Item:%zu req:%d cost:%.2lf\n", i, items[i].r->id, items[i].cost);
		}

	private:
		std::vector< WorstRequestRemovalItem<NodeT> > items;
		IWorstRequestRemovalCost<NodeT, DriverT> * removal_cost;
		WorstRequestRemovalCostNode<NodeT, DriverT> removal_cost_node;
		WorstRequestRemovalCostRequest<NodeT, DriverT> removal_cost_request;
};



#endif
