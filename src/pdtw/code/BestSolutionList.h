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
#ifndef BESTSOLUTIONLIST_H
#define BESTSOLUTIONLIST_H

#include <stdio.h>
#include "Solution.h"
#include "Relatedness.h"
#include "ISolutionList.h"
#include <list>
 

template<class NodeT, class DriverT>
class BestSolutionList : public ISolutionList<NodeT, DriverT> {
public:
    BestSolutionList(Prob<NodeT, DriverT> *prob, int max_count) :
            _prob(prob), _max_list_count(max_count), _list(0), _related_functions(0), _nbitems(0) {}

    ~BestSolutionList() {

        while (_list.size() > 0) {
            sol_list_it it = _list.begin();
            Sol<NodeT, DriverT> *s = *it;
            _list.erase(it);
            delete s;
        }
        _list.clear();
    }

    //pointer sur la solution
    Sol<NodeT, DriverT> *GetSolution(int i) {
        sol_list_it it = _list.begin();
        int j = 1;
        while (j++ < i) it++;
        return *it;
    }

    //mettre sol_list dans v
    void GetSolutions(std::vector<Sol<NodeT, DriverT> *> &v) {
        sol_list_it it = _list.begin();
        for (; it != _list.end(); it++)
            v.push_back(*it);
    }

    int GetSolutionCount() { return (int) _list.size(); }

    Prob<NodeT, DriverT> *GetProblemDefinition() { return _prob; }

    //?????????????
    void Add(Relatedness<NodeT, DriverT> *func) {
        _related_functions.push_back(func);
    }

    void Add(Sol<NodeT, DriverT> &s) {
        double d1 = s.GetLastCalculatedCost();
        if (_list.size() == (size_t) _max_list_count) {
            sol_list_it it = _list.end();
            it--;
            double d2 = (*it)->GetLastCalculatedCost();
            if (d2 <= d1)
                return;
            else {
                Sol<NodeT, DriverT> *sol = *it;
                _list.erase(it);

                for (size_t i = 0; i < _related_functions.size(); i++)
                    _related_functions[i]->Decrease(*sol);
                delete sol;
            }
        }

        for (size_t i = 0; i < _related_functions.size(); i++)
            _related_functions[i]->Increase(s);

        sol_list_it it = _list.begin();
        while (it != _list.end()) {
            double d2 = (*it)->GetLastCalculatedCost();

            //avoid solutions with exactly the same cost
            if (d1 == d2)
                return;
            else if (d1 < d2) {
                Sol<NodeT, DriverT> *newsol = new Sol<NodeT, DriverT>(s);
                _list.insert(it, newsol);
                break;
            }
            it++;
        }
        if (it == _list.end()) {
            Sol<NodeT, DriverT> *newsol = new Sol<NodeT, DriverT>(s);
            _list.push_back(newsol);
        }
    }


    void Add(BestSolutionList<NodeT, DriverT> &list) {
        _max_list_count += list._max_list_count;
        sol_list_it it = list._list.begin();
        while (it != list._list.end()) {
            _list.push_back((*it));
            it++;
        }
        list._list.clear();
    }

    void Show() {
        printf("List of best solutions count:%d\n", (int) _list.size());
        sol_list_it it = _list.begin();
        int i = 0;
        while (it != _list.end()) {
            double d = (*it)->GetLastCalculatedCost();
            printf("i:%d cost:%.3lf\n", i++, d);
            it++;
        }

    }

    void Resize(int size) {
        if (size < _list.size()) {
            printf("Cannot resize this list to a smaller size. Current size:%d new size:%d\n", (int) _list.size(),
                   size);
            exit(1);
        }
        _max_list_count = size;
    }

    int GetSize() { return _max_list_count; }

private:


    Prob<NodeT, DriverT> *_prob;
    int _max_list_count;
    std::list<Sol<NodeT, DriverT> *> _list; //sorted list of solution
    std::vector<Relatedness<NodeT, DriverT> *> _related_functions;
    typedef typename std::list<Sol<NodeT, DriverT> *>::iterator sol_list_it;
    int _nbitems;
};

#endif