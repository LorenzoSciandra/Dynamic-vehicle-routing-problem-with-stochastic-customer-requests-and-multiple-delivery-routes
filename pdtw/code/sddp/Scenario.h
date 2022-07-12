#ifndef _SCENARIO_H
#define _SCENARIO_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "../constants.h"
#include "Decisions.h"
#include "NodeSddp.h"

class Scenario {
public:
    std::vector <Node> nodes;
    int start_time;
    int end_time;

    int GetNodeCount() { return (int) nodes.size(); }

    Node *GetNode(int i) { return &nodes[i]; }

    void AddNode(Node &n) { nodes.push_back(n); }

    double GetNextEvent(double time) {
        double max_event = BIG_DOUBLE;
        for (size_t i = 0; i < nodes.size(); i++)
            if (nodes[i].release_time < max_event && nodes[i].release_time > time + 0.0001)
                max_event = nodes[i].release_time;
        return max_event;
    }
};

#endif
