
#include "RelocateIntraSddp.h"
#include "../../../lib/mathfunc.h"

void RelocateIntraSddp::Optimize(Sol<Node, Driver> &s) {
    exit(1);
}

bool RelocateIntraSddp::Optimize(Sol<Node, Driver> &s, Node *n) {
    Driver *d = s.GetAssignedTo(n);
    if (d == NULL) return false;


    exit(1);
}
