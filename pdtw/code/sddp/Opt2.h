/*
 * Copyright Jean-Francois Cote 2012
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
 *
*/
#ifndef _OPT2_H
#define _OPT2_H

#include "../Solution.h"
#include "../IAlgorithm.h"
#include "../ISolutionList.h"

template <class NodeT, class DriverT>
class Opt2 : public IAlgorithm<NodeT,DriverT>
{

	public:
	void Optimize(Sol<NodeT,DriverT> & s, ISolutionList<NodeT,DriverT> * list)
	{
		Optimize(s);
		if(list != NULL)
		{
			s.Update();
			if(s.IsFeasible())
				list->Add(s);
		}
	}
	void Optimize(Sol<NodeT,DriverT> & s)
	{
		//printf("Opt2 Init Cost:%lf\n", s.GetCost());
		for(int i=0;i<s.GetDriverCount();i++)
		{
			DriverT * d = s.GetDriver(i);
			NodeT * prev = s.GetNode( d->last_done_node_id==-1?d->StartNodeID:d->last_done_node_id );
			while(prev->type != NODE_TYPE_END_DEPOT)
			{
				Node * next = s.GetNext(prev);
				if(next->type == NODE_TYPE_PICKUP)
				{
					prev = next;
					continue;
				}

				while(next->type == NODE_TYPE_DROP)
					next = s.GetNext(next);
				Optimize(s, d, prev, next);
				prev = next;
			}
		}
		//printf("Opt2 Final Cost:%lf\n", s.GetCost());
	}


	//perform a 2-Opt between the start_n and end_n
	//start_n and end_n won't be moved
	void Optimize(Sol<NodeT,DriverT> & s,DriverT * d, NodeT * start_n, NodeT * end_n)
	{
		//printf("2Opt Optimize Driver:%d start:%d end:%d\n", d->id, start_n->id, end_n->id);
		//start_n->Show();
		//end_n->Show();
		//s.Show(d);

		Prob<NodeT,DriverT> * p = s.GetProb();
		bool flag = true;
		while(flag)
		{
			flag = false;

			NodeT * prev1 = start_n;
			NodeT * cur1 = s.GetNext(start_n);
			while(cur1 != end_n)
			{
				NodeT * cur2 = s.GetNext( cur1 );
				while(cur2 != end_n)
				{
					NodeT * next2 = s.GetNext( cur2 );

					double move1 =	p->GetDistance(prev1,cur2) +
							   		p->GetDistance(cur1,next2) -
										p->GetDistance(prev1,cur1) -
							   		p->GetDistance(cur2,next2);

					if(move1 < -0.001)
					{
						s.RevertPath(cur1,cur2);
						flag = true;
						cur1 =  s.GetPrev(end_n);
						break;
					}

					double move2 = 1;
					if(cur2 != s.Next[ cur1->id ])
						move2 =	p->GetDistance(prev1,s.Next[ cur1->id ]) +
							   	p->GetDistance(cur2,cur1) +
									p->GetDistance(cur1,next2) -
									p->GetDistance(prev1,cur1) -
									p->GetDistance(cur1,s.Next[ cur1->id ])-
							   	p->GetDistance(cur2,next2);

					if(move2 < -0.001)
					{
						//double oldcost = s.GetCost(d);

						s.Remove(cur1);
						s.InsertAfter(cur1,cur2);
						/*double newcost = s.GetCost(d);
						if(oldcost + move2 > newcost+0.0001)
						{
							printf("Optimize Error\n");
							exit(1);
						}*/
						flag = true;
						cur1 = s.GetPrev(end_n);
						break;
					}


					cur2 = next2;
				}//end while cur2

				prev1 = cur1;
				cur1 = s.Next[ cur1->id ];
			}//end while cur2
		}//end while flag
	}
};


#endif
