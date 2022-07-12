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

#ifndef REGRET_INSERT_OPERATOR
#define REGRET_INSERT_OPERATOR

#include "InsertOperator.h"
#include "RegretInsertion.h"


template<class NodeT, class DriverT, class MoveT>
class RegretInsertionOperator : public InsertOperator<NodeT, DriverT> {
public:
    RegretInsertionOperator(RegretInsertion<NodeT, DriverT, MoveT> *regret_opt, int k, double noise)
            : _regret_opt(regret_opt), _k(k), _noise(noise) {}

    void Insert(Sol<NodeT, DriverT> &s) {
        _regret_opt->SetK(_k);
        _regret_opt->SetNoise(_noise);
        _regret_opt->Insert(s);
    }

private:
    RegretInsertion<NodeT, DriverT, MoveT> *_regret_opt;
    int _k;
    double _noise;

};

#endif
