//
// Created by stefa on 02/12/22.
//
#include "BBNode.h"

void BBNode::DeclareNode() {
    if (id != -1)
        printf("%d [label=\"Req=%d\"]\n", id, request_id);
}

void BBNode::ToGraph() {
    printf("%d -> %d [label=\"%s\\n%.1lf\"]\n", parent_id, id, DecisionToString().c_str(), cost);
}

int BBNode::ToGraph2(int best_regret_path_node) {
    if (children.empty()) {
        return -2;
    }

    int val = -2;
    BBNode best = children[0];
    // find the best node that would be used by B&R
    for (BBNode &item: children) {
        if (item.cost < best.cost) {
            best = item;
        }
    }

    for (BBNode &item: children) {
        std::string style;
        if (item.id == best.id && best_regret_path_node == id) {
            style = " color=\"red\" penwidth=4";
            val = best.id;
        }

        printf("%d -> %d [label=\"%s\\n%.2lf\"%s]\n", id, item.id, item.DecisionToString().c_str(), item.cost,
               style.c_str());
    }

    return val;
}

std::string BBNode::DecisionToString() {
    std::string decisionTypeString;
    switch (decision_type) {
        case DECISION_ACTION_GO_NOW:
            decisionTypeString = "Go Now";
            break;
        case DECISION_ACTION_WAIT:
            decisionTypeString = "Wait";
            break;
        case DECISION_ACTION_DONT_DELIVER:
            decisionTypeString = "Don't Deliver";
            break;
        default:
            decisionTypeString = std::to_string(decision_type);
            break;
    }

    return decisionTypeString;
}
