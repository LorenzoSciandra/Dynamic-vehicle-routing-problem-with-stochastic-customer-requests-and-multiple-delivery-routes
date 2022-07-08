/*
 * Copyright Jean-Francois Cote 2012
 * 
 * The code may be used for academic, non-commercial purposes only.
 * 
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
 *
 *
 *
 * Produce a list of unique routes. If two routes has the same
 * customers, the route having the smallest cost is kept and the
 * other is discarded.
 *
*/

#ifndef FEASIBLEROUTELIST
#define FEASIBLEROUTELIST


#include "../../lib/primes.h"
#include "Solution.h"
#include <map>
#include <vector>


template <class NodeT, class DriverT>
struct route_elem_t
{
	Sol<NodeT, DriverT> * sol;
	DriverT * drv;
	double route_cost;
	int packing_status;
};

typedef unsigned long long uid_key_t;

template <class NodeT, class DriverT>
class FeasibleRouteList
{
	public:
		FeasibleRouteList(){}
		~FeasibleRouteList(){}
	
		void Add(Sol<NodeT, DriverT> * s,DriverT * d)
		{
			if(s->RoutesLength[d->id] == 0) return;
			
			uid_key_t uid = GetUniqueID(s,d);
			
			map_iter it;
			it = mapped_routes.find(uid);
			if(it != mapped_routes.end())
			{
				double cost = s->GetCost(d);
				route_elem_t<NodeT,DriverT> r = it->second;
				
				if(r.route_cost > cost)
				{
					r.route_cost = cost;
					r.drv = d;
					r.sol = s;
					it->second = r;
				}
				
				//just make sure the added route does not exists
				/*while(it != mapped_routes.end())
				{
					route_elem_t<NodeT,DriverT> r = it->second;
					int re = CompareRoutes(s,d, r.sol, r.drv);
					if(re == 1)
					{
						printf("\t route already exists same id %llu\n", uid);
						//exit(1);	
						break;
					}
					else
					{
						printf("\t route does not exists but same id %llu\t(", uid);
						NodeT * n = r.sol->GetNode(r.drv->StartNodeID);
						while(n != NULL)
						{
							printf("%d ", n->no);
							uid *= Get_Prime_Number(n->no);
							n = r.sol->Next[n->id];
						}
						printf(")\n");
						break;
					}
					it++;
				}*/
			}
			else
			{
				//printf("\t new route\n");
				route_elem_t<NodeT,DriverT> r;
				r.sol = s;
				r.drv = d;
				r.route_cost = s->GetCost(d);
				r.packing_status = 0;
				mapped_routes.insert( std::make_pair (uid, r) );
				feasible_routes.push_back(r);
			}
		}
	
		uid_key_t GetUniqueID(Sol<NodeT, DriverT> * s,DriverT * d)
		{
			uid_key_t uid = 1;
			NodeT * n = s->GetNode(d->StartNodeID);
			while(n != NULL)
			{
				uid *= prime_get_ith(n->no);
				n = s->Next[n->id];
			}
			return uid;
		}
		
		int CompareRoutes(Sol<NodeT, DriverT> * s1,DriverT * d1,Sol<NodeT, DriverT> * s2,DriverT * d2)
		{
			NodeT * n1 = s1->GetNode(d1->StartNodeID);
			NodeT * n2 = s2->GetNode(d2->StartNodeID);
			while(n1 != NULL && n2 != NULL)
			{
				if(n1->id != n2->id) return 0;
				
				n1 = s1->Next[n1->id];
				n2 = s2->Next[n2->id];
			}
			return 1;	
		}
	
		void ShowAllUniqueRoutes()
		{
			printf("All unique routes :%d\n", (int)feasible_routes.size());
			for(size_t i = 0 ; i < feasible_routes.size() ; i++)
				feasible_routes[i].sol->Show(feasible_routes[i].drv);
		}
	
	int GetUniqueRouteCount(){return (int)feasible_routes.size();}
	void GetUniqueRoute(int i, Sol<NodeT, DriverT> ** s,DriverT ** d)
	{
		*s = feasible_routes[i].sol;
		*d = feasible_routes[i].drv;
	}
	
	private:
		std::vector< route_elem_t<NodeT,DriverT> > feasible_routes;
		std::map<unsigned long, route_elem_t<NodeT,DriverT> > mapped_routes;
		typedef typename std::map<unsigned long, route_elem_t<NodeT,DriverT> >::iterator map_iter;
};




#endif
