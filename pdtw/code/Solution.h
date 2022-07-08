/*
 * Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca
 * If you have improvements, please contact me!
 */

#ifndef _SOLUTION_H
#define _SOLUTION_H

#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "../../lib/primes.h"
#include "ProblemDefinition.h"
#include "CostFunction.h"
#include <vector>

template <class NodeT, class DriverT>
class Sol
{

	public:

		Sol(Prob<NodeT, DriverT> * prob,CostFunction<NodeT,DriverT> * cost_func):
										  Next(0),Prev(0),AssignTo(0),
										  RoutesLength(prob->GetDriverCount()),
										  _prob(prob),UnassignedCount(0),
										  Unassigneds(0),UnassignedIndex(0),
										  _cost_func(cost_func), _last_cost(0),_is_feasible(true),_total_distances(0)

		{
			for(int i = 0 ; i < _prob->GetNodeCount() ; i++)
			{
				Next.push_back(NULL);
				Prev.push_back(NULL);
				AssignTo.push_back(NULL);
				Unassigneds.push_back(NULL);
				UnassignedIndex.push_back(-1);
			}

			for(int i = 0 ; i < _prob->GetDriverCount() ; i++)
			{
				RoutesLength[i] = 0;
				DriverT * d = GetDriver(i);
				NodeT * n1 = GetNode(d->StartNodeID);
				NodeT * n2 = GetNode(d->EndNodeID);

				Next[n1->id] = n2;
				Prev[n2->id] = n1;
				AssignTo[n1->id] = d;
				AssignTo[n2->id] = d;
			}
		}


		void AddNode()
		{
			Next.push_back(NULL);
			Prev.push_back(NULL);
			AssignTo.push_back(NULL);
			Unassigneds.push_back(NULL);
			UnassignedIndex.push_back(-1);
		}

		void AddDriver()
		{
			RoutesLength.push_back(0);
		}

		void InsertBefore(NodeT * n, NodeT * next)
		{
			InsertAfter(n, GetPrev(next));
		}
		void InsertAfter(NodeT * n, NodeT * prev)
		{
			RoutesLength[ AssignTo[prev->id]->id ]++;
			AssignTo[n->id] = AssignTo[prev->id];
			Next[n->id] = Next[prev->id];
			Prev[n->id] = prev;
			if(Next[prev->id] != NULL)
				Prev[ Next[prev->id]->id ] = n;

			Next[prev->id] = n;
		}


		void Remove(NodeT * n)
		{
			RoutesLength[ AssignTo[n->id]->id ]--;
			if (Next[n->id] != NULL) {Prev[Next[n->id]->id] = Prev[n->id]; }///i added assignement 2 in if
			if (Prev[n->id] != NULL) {Next[Prev[n->id]->id] = Next[n->id]; }///i added assignement 2 in if
			AssignTo[n->id] = NULL;
		}

		void Remove(Request<NodeT> * r)
		{
			for(int i=0;i<r->GetNodeCount();i++)
				Remove(r->GetNode(i));
		}


		void AddToUnassigneds(NodeT * n)
		{
			if(UnassignedIndex[n->id] == -1)
			{
				UnassignedIndex[n->id] = UnassignedCount;
				Unassigneds[UnassignedCount] = n;
				UnassignedCount++;
			}
		}

		void AddToUnassigneds(Request<NodeT> * r)
		{
			for (int i = 0 ; i<r->GetNodeCount() ; i++)
				AddToUnassigneds(r->GetNode(i));
		}

		//l'ordre ne compte pas
		void RemoveFromUnassigneds(NodeT * n)
		{
			if(UnassignedIndex[n->id] == -1) return;

			int ind = UnassignedIndex[n->id];
			//printf("RemoveFromUnassigneds:%d index:%d count:%d\n", n->id, ind,UnassignedCount);
			if(UnassignedCount > 1)
			{
				NodeT * rep = Unassigneds[UnassignedCount - 1];
				Unassigneds[ind] = rep;
				UnassignedIndex[ rep->id ] = ind;
			}

			Unassigneds[UnassignedCount - 1] = NULL;
			UnassignedCount--;
			UnassignedIndex[n->id] = -1;
		}
		void RemoveFromUnassigneds(Request<NodeT> * r)
		{
			for (int i = 0 ; i<r->GetNodeCount() ; i++)
				RemoveFromUnassigneds(r->GetNode(i));
		}

		void RemoveAndUnassign(NodeT * n)
		{
			Remove(n);
			AddToUnassigneds(n);
		}

		void RemoveAndUnassign(Request<NodeT> * r)
		{
			for(int i=0;i<r->GetNodeCount();i++)
			{
				Remove(r->GetNode(i));
				AddToUnassigneds(r->GetNode(i));
			}
		}

		void UnassignAllCustomers()
		{
			for(int i=0;i<GetCustomerCount();i++)
			{
				NodeT * n = GetCustomer(i);
				if(AssignTo[n->id] != NULL)
					RemoveAndUnassign(n);
			}
		}

		void PutAllNodesToUnassigned()
		{
			for(int i=0;i<GetCustomerCount();i++)
				AddToUnassigneds( GetCustomer(i));
		}

		//Revert the path of n1 to n2 inclusively //more comlicated!!!!!!
		void RevertPath(NodeT * n1, NodeT * n2)
		{
			NodeT * prev = Prev[n1->id];
			NodeT * next = Next[n2->id];

			NodeT * cur = n1;
			NodeT * last = next;
			NodeT * prevlast = n2;
			while(cur != n2)
			{
				NodeT * tmp = Next[cur->id];
				Next[cur->id] = last;
				Prev[last->id] = cur;
				Prev[cur->id] = prevlast;
				Next[prevlast->id] = cur;

				last = cur;
				cur = tmp;
			}

			Next[prev->id] = n2;
			Prev[n2->id] = prev;
		}


		void MakePath(int driver, std::vector<NodeT*> & path)
		{
			DriverT * d = GetDriver(driver);
			NodeT * prev = GetNode(d->StartNodeID);
			for(size_t j=0;j<path.size();j++)
				if((path[j]->type & NODE_TYPE_CUSTOMER) == NODE_TYPE_CUSTOMER)
				{
					if(AssignTo[ path[j]->id ] != NULL)
						Remove( path[j] );
					if( UnassignedIndex[ path[j]->id ] != -1)
						RemoveFromUnassigneds( path[j] );
					InsertAfter(path[j], prev);
					prev = path[j];
				}
		}

		//Make the solution feasible by checking every route and removing the first customers until it is feasible
		void MakeFeasible()
		{
			for(int i=0;i<GetDriverCount();i++)
				_cost_func->MakeFeasible(*this,GetDriver(i));
		}

		//return true if the solution has a route that contains only one customer
		bool ContainSingleCustomerRoute()
		{
			for(int i=0;i<GetDriverCount();i++)
				if(RoutesLength[i] == 1)
					return true;
			return false;
		}


		unsigned long long GetDriverRouteKey(DriverT * d)
		{
			unsigned long long l = 1;
			NodeT * n = GetNode( d->StartNodeID );
			while(n != NULL)
			{
				if(n->type == NODE_TYPE_CUSTOMER)
					l *= prime_get_ith(n->id);
				n = Next[ n->id ];
			}
			return l;
		}


		void RemoveFirstCustomer(DriverT * d)
		{
			NodeT * n = GetNode( d->StartNodeID );
			while(n != NULL)
			{
				if(n->type == NODE_TYPE_CUSTOMER)
				{
					RemoveAndUnassign(n);
					break;
				}
				n = Next[ n->id ];
			}
		}

		void ShowCost()
		{
			printf("Sol cost:%.2lf\n", GetCost());
		}

		void Show()
		{
			_cost_func->Show(this);
			/*double cost = _cost_func->GetCost(*this);
			printf("Solution cost:%.4lf\n", cost );
			for(int i = 0; i<GetDriverCount(); i++)
				//if(RoutesLength[i] >= 1)
					Show(GetDriver(i));
			printf("Unassigneds(%d) :", GetUnassignedCount());
			int k = 0;
			for(int i = 0; i<GetUnassignedCount(); i++)
			{
				printf("%d(rel:%.0lf) ", GetUnassigned(i)->id, GetUnassigned(i)->release_time);
				//printf("%d(rel:%d lw:%d uw:%d)-", GetUnassigned(i)->id, (int)GetUnassigned(i)->release_time,(int)GetUnassigned(i)->tw_start,(int)GetUnassigned(i)->tw_end );
				NodeT * n = GetUnassigned(i);
				Request<NodeT> * r = GetProb()->GetRequest(n);
				if (r->GetParent() == n)
					k++;
			}
			//printf("\nUnassigned requests:%d\n", k);
			printf("\n");*/
		}

		void ShowCost(char cost[250], char DriversUsed[250], char UnassignedReequest[250])
		{
			sprintf(cost, "Solution class:%d cost:%.4lf => ", _prob->GetClass(), _cost_func->GetCost(*this));
			//sprintf(cost, "Solution class:" << _prob->GetClass() << " cost:" << _cost_func->GetCost(*this) << "% .4lf => ");
			//cost = "Solution class:" << _prob->GetClass() << " cost:" << _cost_func->GetCost(*this) << "% .4lf => ";
			int UsedDriver = 0;
			for (int i = 0; i<GetDriverCount(); i++){
				if (RoutesLength[i] >= 1)	UsedDriver++;
			}
			sprintf(DriversUsed, "%d used drivers => ", UsedDriver);
			//DriversUsed = UsedDriver << " used drivers => " ;
			//printf("%d Unassigneds:", GetUnassignedCount());
			int k = 0;
			for (int i = 0; i<GetUnassignedCount(); i++){
				//printf("%d(%d) ", GetUnassigned(i)->origin_id, GetUnassigned(i)->demand);
				NodeT * n = GetUnassigned(i);
				Request<NodeT> * r = GetProb()->GetRequest(n);
				if (r->GetParent() == n)
					k++;
			}
			sprintf(UnassignedReequest, " %d unassigned requests  ::  ",k);
			//UnassignedReequest = k << " unassigned requests\n";
		}

		//�crire une route
		void PrintDriver(int i)
		{
			DriverT* d = GetDriver(i);
			NodeT* n = GetNode(d->StartNodeID);
			printf("route %d : ",d->id);
			while (n != NULL)
			{
				printf("%d (%d):: ", n->id, (int) n->ArrivalTime);
				n = Next[n->id];
			}
			printf("\n");
		}




		void Show(DriverT * d)
		{
			if(_cost_func != NULL)
				_cost_func->Show(this, d);

		}

		//fill the list of nodes from the route i
		void GetRoute(int i, std::vector<NodeT*> & nodes)
		{
			nodes.clear();
			NodeT * cur = GetNode( GetDriver(i)->StartNodeID );
			while(cur != NULL)
			{
				nodes.push_back(cur);
				cur = Next[ cur->id ];
			}
		}

		//fill the list of nodes from the route i
		//with only the customer nodes
		void GetRouteNoDepot(int i, std::vector<NodeT*> & nodes)
		{
			nodes.clear();
			NodeT * cur = GetNode( GetDriver(i)->StartNodeID );
			while(cur != NULL)
			{
				if(cur->type == NODE_TYPE_CUSTOMER)
					nodes.push_back(cur);
				cur = Next[ cur->id ];
			}
		}

		//fill the list of nodes from the route i
		//with only the customer nodes
		void GetIdRouteNoDepot(int i, std::vector<int> & nodes)
		{
			nodes.clear();
			NodeT * cur = GetNode( GetDriver(i)->StartNodeID );
			while(cur != NULL)
			{
				if(cur->type == NODE_TYPE_CUSTOMER)
					nodes.push_back(cur->id);
				cur = Next[ cur->id ];
			}
		}

		void PrintViz()
		{
			int nb = 0;
			for(int i=0;i<GetDriverCount();i++)
				if(RoutesLength[ GetDriver(i)->id ] >= 1)
					nb += RoutesLength[ GetDriver(i)->id ] + 2;

			printf("%d\n",nb);
			for(int i=0;i<GetDriverCount();i++)
				if(RoutesLength[ GetDriver(i)->id ] >= 1)
				{
					NodeT * cur = GetNode( GetDriver(i)->StartNodeID );
					while(cur != NULL)
					{
						printf("%d %lf %lf\n", cur->no, cur->x, cur->y);
						cur = Next[ cur->id ];;
					}
				}
		}

		void PrintVizToFile(char * filename)
		{
			FILE * f = fopen(filename,"w");
			if(f != NULL)
			{
				PrintVizToFile(f);
				fclose(f);
			}
		}

		void PrintVizToFile(FILE * f)
		{
			int nb = 0;
			for(int i=0;i<GetDriverCount();i++)
				if(RoutesLength[ GetDriver(i)->id ] >= 1)
					nb += RoutesLength[ GetDriver(i)->id ] + 2;

			fprintf(f,"%d\n",nb);
			for(int i=0;i<GetDriverCount();i++)
				if(RoutesLength[ GetDriver(i)->id ] >= 1)
				{
					NodeT * cur = GetNode( GetDriver(i)->StartNodeID );
					while(cur != NULL)
					{
						fprintf(f,"%d %lf %lf\n", cur->no, cur->x, cur->y);
						cur = Next[ cur->id ];;
					}
				}
		}


		void PrintToFile(FILE * f)
		{
			Update();
			int no_route = 1;
			fprintf(f,"Solution class:%d cost:%.4lf\n", _prob->GetClass(), _cost_func->GetCost(*this) );
			for(int i=0;i<GetDriverCount();i++)
				if(RoutesLength[ GetDriver(i)->id ] >= 1)
				{
					fprintf(f,"Route:%d\n", no_route++);
					NodeT * cur = GetNode( GetDriver(i)->StartNodeID );
					while(cur != NULL)
					{
						//fprintf(f,"%d (req:cur->Num_Request) -", cur->id);
						cur->PrintToFile(f);
						cur = Next[ cur->id ];
					}
					fprintf(f,"\n");
				}
			fprintf(f,"Unassigneds:");
			for(int i=0;i<GetUnassignedCount();i++)
				fprintf(f,"%d-", GetUnassigned(i)->origin_id);
			fprintf(f,"\n");
		}

		bool IsAssigned(NodeT * n){ return AssignTo[n->id] != NULL;}
		DriverT* GetAssignedTo(NodeT * n){ return AssignTo[n->id];}
		DriverT* GetAssignedTo(Request<NodeT> * r){ return AssignTo[r->GetParent()->id];}

		int GetCustomerCount(){ return _prob->GetCustomerCount();}
		NodeT * GetCustomer(int i){ return _prob->GetCustomer(i);}

		int GetNodeCount(){ return _prob->GetNodeCount();}
		NodeT * GetNode(int i){ return _prob->GetNode(i);}

		int GetDriverCount(){ return _prob->GetDriverCount();}
		DriverT * GetDriver(int i){ return _prob->GetDriver(i);}

		int GetRequestCount(){ return _prob->GetRequestCount();}
		Request<NodeT> *  GetRequest(int r){ return _prob->GetRequest(r);}
		Request<NodeT> *  GetRequest(NodeT * n){ return _prob->GetRequest(n);}

		int GetUsedDriverCount()
		{
			int nb = 0;
			for(int i=0;i<GetDriverCount();i++)
				if( RoutesLength[ GetDriver(i)->id ] >= 1)
					nb++;
			return nb;
		}

		int GetUnassignedCount(){ return UnassignedCount;}
		NodeT * GetUnassigned(int i){ return Unassigneds[i];}

		int GetUnassignedRequestCount()
		{
			int nb = 0;
			for(int i=0;i<GetUnassignedCount();i++)
			{
				NodeT * n = GetUnassigned(i);
				Request<NodeT> * r = _prob->GetRequest(n);
				if(r->GetParent() == n)
					nb++;
			}
			return nb;
		}

		double GetCost(){ _last_cost = _cost_func->GetCost(*this); return _last_cost;}
		double GetCost(DriverT * d){ return _cost_func->GetCost(*this,d);}
		double GetCost(int i){ return _cost_func->GetCost(*this,GetDriver(i));}
		double GetLastCalculatedCost(){ return _last_cost; }
		CostFunction<NodeT,DriverT> * GetCostFunction(){ return _cost_func; }

		bool IsFeasible(){return _is_feasible;}
		void SetIsFeasible(bool f){_is_feasible = f;}

		double Update(){ return _cost_func->Update(*this);}
		double Update(DriverT * d){ return _cost_func->Update(*this,d);}

		double GetTotalDistances(){return _total_distances;}
		void SetTotalDistances(double d){_total_distances = d;}

		bool IsUnassigned(NodeT * n){return UnassignedIndex[n->id] != -1;}

		Prob<NodeT, DriverT> * GetProblemDefinition(){return _prob;}
		Prob<NodeT, DriverT> * GetProb(){return _prob;}

		double ** GetDistances(){ return _prob->GetDistances();}
		double ** GetTimes(){ return _prob->GetTimes();}
		double GetTime(int dist_id_from, int dist_id_to){return _prob->GetTimes()[dist_id_from][dist_id_to];}
		double GetTime(NodeT * n1, NodeT * n2){return _prob->GetTimes()[n1->distID][n2->distID];}
		double GetDist(NodeT * n1, NodeT * n2){return _prob->GetDistances()[n1->distID][n2->distID];}

		NodeT* GetNext(NodeT * n){return Next[n->id];}
		NodeT* GetPrev(NodeT * n){return Prev[n->id];}
		DriverT* GetDriver(NodeT * n){return AssignTo[n->id];}
		int GetRouteLength(DriverT * d){return RoutesLength[d->id];}
		int GetRouteLength(int i){return RoutesLength[i];}

		std::vector<NodeT*> Next;
		std::vector<NodeT*> Prev;
		std::vector<DriverT*> AssignTo;
		std::vector<int> RoutesLength;//# of customers in each route

	private:
		Prob<NodeT, DriverT> * _prob;
		int UnassignedCount;
		std::vector<NodeT*> Unassigneds;
		std::vector<int> UnassignedIndex;
		CostFunction<NodeT,DriverT> * _cost_func;

		double _last_cost;
		bool _is_feasible;
		double _total_distances;
};

#endif
