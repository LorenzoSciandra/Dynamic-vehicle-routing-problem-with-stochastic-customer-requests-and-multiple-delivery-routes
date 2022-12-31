/*
 * Copyright Jean-Francois Cote 2019
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
*/

#ifndef RELATEDNESS_SDDP_H
#define RELATEDNESS_SDDP_H

#include "../Solution.h"
#include "../Relatedness.h"
#include "NodeSddp.h"
#include "DriverSddp.h"


class RelatednessSddp : public Relatedness<Node, Driver> {
public:
    RelatednessSddp(Prob<Node, Driver> &pr) : _m(pr.GetDistances()), _prob(pr) {}

    ~RelatednessSddp() {}

    double GetRelatedness(Node *n1, Node *n2) {
        return _m[n1->distID][n2->distID];
    }

    double GetScore(Sol<Node, Driver> &s, Node *n1) {
        return 0;
    }

    //for those with memory
    void Increase(Sol<Node, Driver> &s) {}

    void Decrease(Sol<Node, Driver> &s) {}

private:
    double **_m;
    Prob<Node, Driver> &_prob;
};

#endif
