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

#ifndef RELATEDNESS_H
#define RELATEDNESS_H

#include "Solution.h"

template<class NodeT, class DriverT>
class Relatedness {
public:
    Relatedness() {}

    virtual ~Relatedness() {}

    virtual double GetRelatedness(NodeT *n1, NodeT *n2) = 0;

    virtual double GetScore(Sol<NodeT, DriverT> &s, NodeT *n1) = 0;

    //for those with memory
    virtual void Increase(Sol<NodeT, DriverT> &s) = 0;

    virtual void Decrease(Sol<NodeT, DriverT> &s) = 0;
};

template<class NodeT, class DriverT>
struct RelatedPairSorter {
    NodeT *selectedNode;
    Relatedness<NodeT, DriverT> *rel;

    bool operator()(NodeT *n1, NodeT *n2) {
        return (rel->GetRelatedness(n1, selectedNode) < rel->GetRelatedness(n2, selectedNode));
    }
};

template<class NodeT, class DriverT>
struct RelatedPairSorterRequest {
    NodeT *selectedNode;
    Relatedness<NodeT, DriverT> *rel;

    bool operator()(Request<NodeT> *r1, Request<NodeT> *r2) {
        return (rel->GetRelatedness(r1->GetParent(), selectedNode) <
                rel->GetRelatedness(r2->GetParent(), selectedNode));
    }
};

#endif
