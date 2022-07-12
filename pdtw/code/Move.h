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

#ifndef MOVE_H_
#define MOVE_H_

#include <stdlib.h>
#include "../../lib/mathfunc.h"

template<class NodeT, class DriverT, class MoveT>
class Move {
public:
    NodeT *n;
    DriverT *to;
    DriverT *from;
    double DeltaCost;
    double DeltaDistance;
    bool IsFeasible;
    MoveT move;
    NodeT *nearest;
    NodeT *last;


    Move() : n(NULL), to(NULL), from(NULL), DeltaCost(INFINITE), IsFeasible(false) {}

    void Init() {
        DeltaCost = INFINITE;
        IsFeasible = false;
    }
};


#endif
