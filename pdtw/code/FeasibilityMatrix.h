/*
	Indicate if two nodes can be in the same route
	Jean-Francois Cote July 2017
*/

#ifndef FEASIBILITY_MATRIX
#define FEASIBILITY_MATRIX



#include <vector>
class FeasibilityMatrix
{
	public:
		FeasibilityMatrix(int nb_nodes)
		{
			_is_feasible.resize( nb_nodes );
			for(int i=0;i<nb_nodes;i++)
				_is_feasible[i].resize(nb_nodes, true);
		}

		bool IsFeasible(int n1, int n2){return _is_feasible[n1][n2];}
		void SetFeasibility(int n1, int n2, bool b)
		{
			_is_feasible[n1][n2] = b;
			_is_feasible[n2][n1] = b;
		}
	private:
		std::vector< std::vector<bool> > _is_feasible;
};

#endif
