/*
 * Copyright Jean-Francois Cote 2016
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca
 * If you have improvements, please contact me!
 */

#ifndef ALNS_METHOD1
#define ALNS_METHOD1

#include "IAlgorithm.h"
#include "InsertOperator.h"
#include "RemoveOperator.h"
#include "Solution.h"
#include "../../lib/mathfunc.h"
#include "../../lib/Chrono.h"
#include "ISolutionList.h"
#include <math.h>

template<class NodeT, class DriverT>
struct alns_remove_operator
{
	int no;
	double w;
	double nb_selected;
	double score;
	int nb;
	int nbbest;
	double interval1;
	double interval2;
	RemoveOperator<NodeT,DriverT> * opt;
};

template<class NodeT, class DriverT>
struct alns_insert_operator
{
	InsertOperator<NodeT, DriverT> * opt;
	int no;
	double w;
	double nb_selected;
	double score;
	int nb;
	int nbbest;
	double interval1;
	double interval2;
};



template <class NodeT, class DriverT>
class ALNS : public IAlgorithm<NodeT,DriverT>
{
	public:
		ALNS()
		{
			_iterator_count = 25000;
			_temperature = 0.9996;
			_percentage_max = 0.4;_percentage_min = 0.1;
			_max_nb_items = 60; 	_min_nb_items = 30;
			_sigma1 = 4; _sigma2 = 1; _sigma3 = 0; _sigma4 = 0;
			_floor_min_items = 3;
			_p = 0.05;
			_temperature_iter_init = 0;

			_max_time = 99999999;
			_chrono_check_iter = 1000;
			show_output = true;
			_local_search_on_new_best = false;
			_local_search_on_score_update = false;
			_stop_when_all_assigned = false;
		}

		void Optimize(Sol<NodeT, DriverT> & s)
		{
			Optimize(s, NULL);
		}
		void Optimize(Sol<NodeT, DriverT> & s, ISolutionList<NodeT,DriverT> * best_sol_list)
		{
			Sol<NodeT, DriverT> best = s;
			Sol<NodeT, DriverT> cur = s;
			double best_cost = s.GetCost();
			if(best_sol_list != NULL && s.IsFeasible())
				best_sol_list->Add(s);

			s.Update();
			double init_dist = s.GetTotalDistances();
			double curr_cost = best_cost;

			_chrono.start();
			_chrono.setDuration(_max_time);

			int nb_best_new = 0;
			int nb_improved = 0;
			int nb_improved_accepted = 0;
			int nb_worst = 0;
			int nb_worst_accepted = 0;
			int nb_worst_with_unassigneds = 0;
			double avg_gap_worst = 0;
			double avg_gap_worst_accepted = 0;
			double avg_gap_worst_refused = 0;
			double avg_gap_improved = 0;
			double avg_gap_best = 0;

			Init();

			int nb_iter_without_new_best = 0;

			double w = 0.05;
			double Tmin = init_dist * (1+w) * pow(_temperature, _temperature_iter_init);
			double Tmax = init_dist * (1+w) * pow(_temperature, 30000);
			double T = Tmin;
			if(show_output)printf("ALNS it:%d Tmin:%.3lf TMax:%.3lf ",_iterator_count, Tmin,Tmax);
			if(show_output)printf("init_dist:%.3lf pow:%.3lf T:%.3lf\n",init_dist,pow(_temperature, 30000),_temperature);
			for(int iter = 0 ; iter < _iterator_count ;iter++)
			{
				int nbitems = s.GetRequestCount() - s.GetUnassignedRequestCount();
				int nbMaxRemove = MIN_INT(_max_nb_items, (int)(nbitems*_percentage_max));
				int nbMinRemove = MIN_INT(_min_nb_items, (int)(nbitems*_percentage_min));
				int nbremove =  (nbMinRemove == nbMaxRemove)? nbMaxRemove : mat_func_get_rand_int(nbMinRemove, nbMaxRemove);
				nbremove = MIN_INT(MAX_INT(nbremove, _floor_min_items),nbitems);

				alns_remove_operator<NodeT,DriverT> * rmv = GetRemoveOperator();
				alns_insert_operator<NodeT,DriverT> * ins = GetInsertOperator();

				ins->nb++;			rmv->nb++;
				ins->nb_selected++; rmv->nb_selected++;

				rmv->opt->Remove(cur, nbremove);
				ins->opt->Insert(cur);
				//DoLocalSearch(cur, best_sol_list);
				//ins->opt->Insert(cur);
				//printf("Iter:%d\n", iter);
				double newcost = cur.GetCost();
				if(show_output && (iter % 1000) == 0)// || best_cost > newcost)
				printf("Iter:%d rmv:%d newcost:%.2lf(%d,%d) cost:%.2lf best:%.2lf T:%.8lf\n",
						iter,nbremove,newcost,cur.GetUnassignedCount(),(int)cur.IsFeasible(),curr_cost, best_cost,T);


				if(best_sol_list != NULL && cur.IsFeasible()) best_sol_list->Add(cur);

				double gap = (newcost - curr_cost) / curr_cost;
				double gap_best = (newcost - best_cost) / best_cost;


				if(best_cost > newcost && cur.IsFeasible())
				{
					if(_local_search_on_new_best)
					{
						DoLocalSearch(cur, best_sol_list);
						newcost = cur.GetCost();
					}

					nb_iter_without_new_best = 0;
					nb_best_new++;
					avg_gap_best += gap;

					best_cost = newcost;
					curr_cost = newcost;
					best = cur;
					s = cur;
					ins->score += _sigma1; 	rmv->score += _sigma1;
					ins->nbbest++;	rmv->nbbest++;

					if(_stop_when_all_assigned && cur.GetUnassignedCount() == 0)
						break;
					//s.Show();
				}
				else
				{
					nb_iter_without_new_best++;
					double prob = mat_func_get_rand_double();
					double acpprob = exp((curr_cost-newcost)/T);
					if(curr_cost > newcost) {nb_improved++;avg_gap_improved += gap;}
					else					{nb_worst++; if(cur.GetUnassignedCount() == 0) avg_gap_worst += gap;}

					if(cur.GetUnassignedCount() >= 1) nb_worst_with_unassigneds++;
					//printf("prob:%lf acpprob:%lf T:%lf cost:%lf prevcost:%lf best:%lf\n", prob, acpprob, T, cost, costcur, bestcost);
					if(prob < acpprob && gap_best <= 0.05)
					//if(prob < acpprob)
					{
						s = cur;

						if(newcost < curr_cost)
						{
							nb_improved_accepted++;
							ins->score += _sigma2;
							rmv->score += _sigma2;
							curr_cost = newcost;
						}
						else
						{
							if(cur.GetUnassignedCount() == 0) avg_gap_worst_accepted += gap;
							nb_worst_accepted++;
							ins->score += _sigma3;
							rmv->score += _sigma3;
						}

					}
					else
					{
						if(cur.GetUnassignedCount() == 0) avg_gap_worst_refused += gap;
						cur = s;
					}
				}//end else

				if(iter % 100 == 0)
				{
					UpdateScores();
					if(_local_search_on_score_update)
					{
						DoLocalSearch(cur, best_sol_list);
						newcost = cur.GetCost();
						if(best_cost > newcost && cur.IsFeasible())
						{
							nb_iter_without_new_best = 0;
							nb_best_new++;
							best_cost = newcost;
							curr_cost = newcost;
							best = cur;
							s = cur;
						}
					}
				}
				if(iter % _chrono_check_iter == 0 && _chrono.hasToEnd()) break;

				T = T * _temperature;
				if(T < Tmax || T < 0.000001)// && nb_iter_without_new_best >= 10000)
					T= Tmin;

			}//end for

			//printf("Nb BestNew:%d\n", nb_best_new);
			//printf("Nb Improved:%d\n", nb_improved);
			//printf("Nb Improved Accepted:%d\n", nb_improved_accepted);
			//printf("Nb Worst:%d\n", nb_worst);
			//printf("Nb Worst Accepted:%d\n", nb_worst_accepted);
			//printf("Nb Worst Refused:%d\n", nb_worst-nb_worst_accepted);
			//printf("nb_worst_with_unassigneds:%d\n", nb_worst_with_unassigneds);
			//
			//printf("avg_gap_worst:%.5lf\n", avg_gap_worst/nb_worst);
			//printf("avg_gap_worst_accepted:%.5lf\n", avg_gap_worst_accepted/nb_worst_accepted);
			//printf("avg_gap_worst_refused:%.5lf\n", avg_gap_worst_refused/(nb_worst-nb_worst_accepted));
			//printf("avg_gap_improved:%.5lf\n", avg_gap_improved/nb_improved);
			//printf("avg_gap_best:%.5lf\n", avg_gap_best/nb_best_new);
			//
			//for(size_t j=0;j<insert_operators.size();j++)
			//	printf("Insertion Operator:%d Best:%d Nb:%d\n", (int)j, insert_operators[j].nbbest, insert_operators[j].nb);
			//
			//for(size_t j=0;j<remove_operators.size();j++)
			//	printf("Remove Operator:%d Best:%d Nb:%d\n", (int)j, remove_operators[j].nbbest, remove_operators[j].nb);
			//*/
			s = best;
			//best.Show();
		}//end Optimize

		void CallLocalSearchOnNewBest(bool v){ _local_search_on_new_best = v;}
		void CallLocalSearchOnScoreUpdate(bool v){ _local_search_on_score_update = v;}
		void SetItemMinRemoved(int minp){_min_nb_items = minp;}
		void SetItemMaxRemoved(int maxp){_max_nb_items = maxp;}
		void SetPercentageMin(double minp){_percentage_min = minp;}
		void SetPercentageMax(double maxp){_percentage_max = maxp;}
		void SetTemperatureIterInit(double temp){_temperature_iter_init = temp;}
		void SetTemperature(double temp){_temperature = temp;}
		void SetIterationCount(int it){_iterator_count = it;}
		void SetMaxTime(int maxtime){_max_time = maxtime;}
		void SetChronoCheckIter(int iter){ _chrono_check_iter = iter;}
		void StopWhenAllAssigned(){_stop_when_all_assigned = true;}
		void AddInsertOperator(InsertOperator<NodeT, DriverT> * opt)
		{
			alns_insert_operator<NodeT, DriverT> o;
			o.opt = opt;
			insert_operators.push_back(o);
		}
		void AddRemoveOperator(RemoveOperator<NodeT, DriverT> * opt)
		{
			alns_remove_operator<NodeT, DriverT> o;
			o.opt = opt;
			remove_operators.push_back(o);
		}

		void AddLocalSearchAlgorithm(IAlgorithm<NodeT,DriverT>* algo)
		{
			_local_search_algorithms.push_back(algo);
		}

		bool show_output;
	private:

		void Init()
		{
			for(size_t j=0;j<insert_operators.size();j++)
			{
				insert_operators[j].w = 0;
				insert_operators[j].nb_selected=1;
				insert_operators[j].score=1;
				insert_operators[j].nb=0;
				insert_operators[j].nbbest=0;
			}
			for(size_t j=0;j<remove_operators.size();j++)
			{
				remove_operators[j].w = 0;
				remove_operators[j].nb_selected=1;
				remove_operators[j].score=1;
				remove_operators[j].nb=0;
				remove_operators[j].nbbest=0;
			}
		}

		void UpdateScores()
		{
			size_t j;
			for(j=0;j<insert_operators.size();j++)
			{
				insert_operators[j].w = 	insert_operators[j].w * (1 - _p) +
											insert_operators[j].score/insert_operators[j].nb_selected*_p;
				insert_operators[j].nb_selected=1;
				insert_operators[j].score=0;
			}
			for(j=0;j<remove_operators.size();j++)
			{
				remove_operators[j].w = 	remove_operators[j].w * (1 - _p) +
											remove_operators[j].score/remove_operators[j].nb_selected*_p;
				remove_operators[j].nb_selected=1;
				remove_operators[j].score=0;
			}
		}

		alns_remove_operator<NodeT, DriverT> * GetRemoveOperator()
		{
			double sumw = 0;
			size_t i;
			for(i=0;i<remove_operators.size();i++)
				sumw += remove_operators[i].w;

			double interval = 0;
			for(i=0;i<remove_operators.size();i++)
			{
				remove_operators[i].interval1 = interval;
				interval += remove_operators[i].w/sumw;
				remove_operators[i].interval2 = interval;
			}

			double k = mat_func_get_rand_double();
			for(i=0;i<remove_operators.size();i++)
				if(remove_operators[i].interval1 <= k && k <= remove_operators[i].interval2)
					return &remove_operators[i];
			return &remove_operators[remove_operators.size()-1];
		}
		alns_insert_operator<NodeT, DriverT> * GetInsertOperator()
		{
			double sumw = 0;
			size_t i;
			for(i=0;i<insert_operators.size();i++)
				sumw += insert_operators[i].w;

			double interval = 0;
			for(i=0;i<insert_operators.size();i++)
			{
				insert_operators[i].interval1 = interval;
				interval += insert_operators[i].w/sumw;
				insert_operators[i].interval2 = interval;
			}

			double k = mat_func_get_rand_double();
			for(i=0;i<insert_operators.size();i++)
				if(insert_operators[i].interval1 <= k && k <= insert_operators[i].interval2)
					return &insert_operators[i];
			return &insert_operators[insert_operators.size()-1];
		}

		void DoLocalSearch(Sol<NodeT,DriverT> & sol, ISolutionList<NodeT,DriverT> * best_sol_list)
		{
			for(size_t i=0;i<_local_search_algorithms.size();i++)
			{
				if(best_sol_list == NULL)
					_local_search_algorithms[i]->Optimize(sol);
				else
					_local_search_algorithms[i]->Optimize(sol,best_sol_list);
			}
		}


	int _iterator_count;
	double 	_temperature;
	double 	_percentage_max;
	double 	_percentage_min;
	int 	_max_nb_items;
	int 	_min_nb_items;
	int _floor_min_items;
	double 	_sigma1;
	double 	_sigma2;
	double 	_sigma3;
	double 	_sigma4;
	double 	_p;
	double _temperature_iter_init;
	std::vector<alns_insert_operator<NodeT, DriverT> > insert_operators;
	std::vector<alns_remove_operator<NodeT, DriverT> > remove_operators;
	int _max_time;			//max resolution time
	int _chrono_check_iter; //number of iteration before a check of the chrono (might cost alot)
	ChronoCpuNoStop _chrono;
	bool _local_search_on_new_best;
	bool _local_search_on_score_update;
	bool _stop_when_all_assigned;

	std::vector< IAlgorithm<NodeT,DriverT>*	> _local_search_algorithms;

};



#endif
