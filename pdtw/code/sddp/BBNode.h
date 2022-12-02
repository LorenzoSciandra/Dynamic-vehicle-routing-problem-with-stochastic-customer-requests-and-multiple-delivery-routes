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
    int id = -1;
    int parent_id;
    int decision_type;
    int request_id;
    double cost;
    std::vector<BBNode> children;

    BBNode() {};

    void DeclareNode();

    void ToGraph();

    std::string DecisionToString();

    int ToGraph2(int best_regret_path_node);
};

#endif //SDDP_APP_BBNODE_H
