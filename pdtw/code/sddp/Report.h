/*
 * Copyright Jean-Francois Cote 2022
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
*/

#ifndef REPORT_SDDP
#define REPORT_SDDP

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../Solution.h"
#include "../ProblemDefinition.h"
#include "NodeSddp.h"
#include "DriverSddp.h"

class Report {

public:
    void Show();

    void Build(Sol<Node, Driver> &s);

    double GetIdleRate();

    std::vector<int> drivers_at_depot;
    std::vector<int> waiting_at_delivery;


private:
    void GetDriverCountAtDepot(Sol<Node, Driver> &s);

    void GetWaitingAtDelivery(Sol<Node, Driver> &s);
};

#endif
