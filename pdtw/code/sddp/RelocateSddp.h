/*
 * Copyright Jean-Francois Cote 2015
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
 */
#ifndef _RELOCATE_SDDP_H
#define _RELOCATE_SDDP_H

#include "../Solution.h"
#include "../IAlgorithm.h"
#include "../InsRmvMethod.h"

#include <vector>
#include "NodeSddp.h"
#include "DriverSddp.h"
#include "InsRmvMethodSddp.h"

class RelocateSddp : public IAlgorithm<Node, Driver> {

public:
    RelocateSddp(InsRmvMethod<Node, Driver, MoveSddp> *insrmv) : _insrmv(insrmv) {}

    void Optimize(Sol<Node, Driver> &s, ISolutionList<Node, Driver> *list) {
        Optimize(s);
        if (list != NULL && s.IsFeasible())
            list->Add(s);
    }

    void Optimize(Sol<Node, Driver> &s);

private:
    InsRmvMethod<Node, Driver, MoveSddp> *_insrmv;
    std::vector<Request<Node> *> requests;
};


#endif
