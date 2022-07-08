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




#ifndef REMOVE_RELATED_QUICK
#define REMOVE_RELATED_QUICK

#include "Solution.h"
#include "../../lib/mathfunc.h"
#include "RemoveOperator.h"
#include "Relatedness.h"
#include <vector>
#include <algorithm>


template< class NodeT, class DriverT>
class RemoveRelatedQuick : public RemoveOperator<NodeT,DriverT>
{
	public:
		RemoveRelatedQuick(Relatedness<NodeT,DriverT> * rel):relatedness(rel){}
		~RemoveRelatedQuick(){}

		void Remove(Sol<NodeT, DriverT> & s, int count)
		{
			//printf("RemoveRelated\n");
			vect.clear();
			for (int i = 0; i < s.GetRequestCount(); i++)
			{
				Request<NodeT>* r = s.GetRequest(i);
				if (s.GetAssignedTo(r->GetParent()) != NULL)
					vect.push_back(r);
			}
			if(vect.size() <= 1) return;

			int size = vect.size() - 1;
			int index = mat_func_get_rand_int(0, vect.size());
			Request<NodeT> * r = vect[index];
			s.RemoveAndUnassign(r);
			vect[index] = vect[size];

			RelatedPairSorterRequest<NodeT,DriverT> mysorter;
			mysorter.rel = relatedness;
			mysorter.selectedNode = r->GetNode(0);

			int cpt = std::min(count-1, (int)vect.size()-2);
			//std::sort(vect.begin(), vect.begin()+size, mysorter);
			std::partial_sort(vect.begin(),vect.begin()+std::min(2*cpt, size),vect.begin()+size, mysorter);


			size_t pos = 0;
			while(cpt > 0 && pos < size)
			{
				double r = 	mat_func_get_rand_double();
				if(r <= 0.9)
				{
					s.RemoveAndUnassign( vect[pos] );
					cpt--;
				}
				pos++;
			}
		}
	private:
		std::vector< Request<NodeT>* > vect;
		Relatedness<NodeT,DriverT> * relatedness;


};


#endif
