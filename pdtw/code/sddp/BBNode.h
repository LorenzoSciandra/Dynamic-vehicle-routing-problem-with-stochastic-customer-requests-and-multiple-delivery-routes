//
// Created by stefa on 02/12/22.
//

#ifndef SDDP_APP_BBNODE_H
#define SDDP_APP_BBNODE_H

#include <stdio.h>
#include <vector>
#include "Report.h"
#include "../SolCompact.h"
#include "Parameters.h"
#include "Test.h"
#include "DecisionMultiSet.h"
#include "Decisions.h"
#include "Scenarios.h"

class BBNode {
public:
    unsigned long id = -1;
    unsigned long parent_id;
    int decision_type;
    int request_id;
    double cost;
    int tree_level = 0;
    // The edge that points to THIS node.
    bool edge_best = false;
    bool edge_regret = false;
    std::vector<BBNode> children;

    BBNode() {};

    void DeclareNode();

    void ToGraph();

    std::string DecisionToString();

    int ToGraph(int best_regret_path_node);

};

#endif //SDDP_APP_BBNODE_H
