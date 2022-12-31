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

#ifndef ISOLUTIONLIST
#define ISOLUTIONLIST

#include "Solution.h"
#include <vector>

template<class NodeT, class DriverT>
class ISolutionList {
public :
    ISolutionList() {}

    virtual ~ISolutionList() {}

    //Add a solution to the list
    virtual void Add(Sol<NodeT, DriverT> &s) = 0;

    //return the number of solutions
    virtual int GetSolutionCount() = 0;

    //return the ith solution (depending on the data structure it may be more efficient to use GetSolutions())
    virtual Sol<NodeT, DriverT> *GetSolution(int i) = 0;

    //take all the solutions and put them in the vector v
    virtual void GetSolutions(std::vector<Sol<NodeT, DriverT> *> &v) {}

    //Show all the solutions
    virtual void Show() {}

    //resize the list
    virtual void Resize(int size) {}

    virtual int GetSize() { return 0; }

    //Add all the solutions from list2 into this list
    virtual void Add(ISolutionList<NodeT, DriverT> *list2) {
        std::vector < Sol<NodeT, DriverT> * > v;
        list2->GetSolutions(v);
        for (size_t i = 0; i < v.size(); i++)
            Add(*v[i]);
    }

};


#endif
