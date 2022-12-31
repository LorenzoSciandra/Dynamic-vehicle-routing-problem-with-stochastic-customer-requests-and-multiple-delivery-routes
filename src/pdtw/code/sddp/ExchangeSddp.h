/*
 * Copyright Jean-Francois Cote 2015
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
 */
#ifndef EXCHANGE_SDDP
#define EXCHANGE_SDDP

#include "../Solution.h"
#include "../IAlgorithm.h"
#include "../InsRmvMethod.h"
#include "NodeSddp.h"
#include "DriverSddp.h"
#include "InsRmvMethodSddp.h"


class ExchangeSddp : public IAlgorithm<Node, Driver> {

public:
    ExchangeSddp(InsRmvMethod<Node, Driver, MoveSddp> *insrmv) : _insrmv(insrmv) {}


    void Optimize(Sol<Node, Driver> &s);

private:
    InsRmvMethod<Node, Driver, MoveSddp> *_insrmv;
    std::vector<Request<Node> *> requests;
};


#endif
