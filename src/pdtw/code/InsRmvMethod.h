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
#ifndef INSRMV_METHOD
#define INSRMV_METHOD

#include "Solution.h"
#include "ProblemDefinition.h"
#include "Move.h"


template<class NodeT, class DriverT, class MoveT>
class InsRmvMethod {
public:
    InsRmvMethod() : _noise(0) {}

    virtual ~InsRmvMethod() {}

    virtual void InsertCost(Sol<NodeT, DriverT> &s, NodeT *n, DriverT *d, Move<NodeT, DriverT, MoveT> &m) = 0;

    virtual void InsertCost(Sol<NodeT, DriverT> &s, Request<NodeT> *r, DriverT *d, Move<NodeT, DriverT, MoveT> &m) {
        InsertCost(s, r->GetParent(), d, m);
    }

    virtual void ApplyInsertMove(Sol<NodeT, DriverT> &s, Move<NodeT, DriverT, MoveT> &m) = 0;

    virtual void RemoveCost(Sol<NodeT, DriverT> &s, NodeT *n, Move<NodeT, DriverT, MoveT> &m) = 0;

    virtual void RemoveCost(Sol<NodeT, DriverT> &s, Request<NodeT> *r, Move<NodeT, DriverT, MoveT> &m) {
        RemoveCost(s, r->GetParent(), m);
    }

    void SetNoise(double noise) { _noise = noise; }

    double GetNoise() { return _noise; }

private:
    double _noise;
};

#endif
