//
// Created by stefa on 02/12/22.
//
#include "BBNode.h"

void BBNode::DeclareNode() {
    if (id != -1)
        printf("%d [label=\"Req=%d\"]\n", id, request_id);
}

void BBNode::ToGraph() {
    std::string style;

    if (edge_best && !edge_regret) {
        style = " color=\"blue\" penwidth=4";
    } else if (!edge_best && edge_regret) {
        style = " color=\"red\" penwidth=4";
    } else if (edge_best && edge_regret) {
        style = " color=\"red:blue\" penwidth=4";
    }

    printf("%d -> %d [label=\"%s\\n%.2lf\"%s]\n", parent_id, id, DecisionToString().c_str(), cost, style.c_str());
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