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


#ifndef REMOVE_RANDOM_SDDP
#define REMOVE_RANDOM_SDDP

#include "../Solution.h"
#include "../../../lib/mathfunc.h"
#include "../RemoveOperator.h"
#include <vector>
#include <algorithm>

template< class NodeT, class DriverT>
class RemoveRandomSddp : public RemoveOperator<NodeT,DriverT>
{
	public:
		RemoveRandomSddp(){}
		~RemoveRandomSddp(){}

		void Remove(Sol<NodeT, DriverT> & s, int count)
		{
			requests.clear();
			for(int i = 0 ; i < s.GetRequestCount() ; i++)
				if(s.GetAssignedTo(s.GetProb()->GetRequest(i)) != NULL && !s.GetProb()->GetRequest(i)->GetPickupNode(0)->IsFixed() )
					requests.push_back(s.GetProb()->GetRequest(i));

			int cpt = MIN_INT(count, (int)requests.size());
			for(int i = 0 ; i < cpt ; i++)
			{
				int index = mat_func_get_rand_int(0, requests.size() - i);
				s.RemoveAndUnassign( requests[index] );
				requests[index] = requests[ requests.size() - i - 1];
			}
			s.MakeFeasible();
		}
	private:
		std::vector< Request<NodeT>* > requests;
};

#endif
