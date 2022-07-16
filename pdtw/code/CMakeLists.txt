cmake_minimum_required(VERSION 3.13)
project(sddp_app)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "-O3 -m64")

SET(UD ../../lib)
# mathfunc.o util.o primes.o bitset.o bag.o Network.o UbManager.o constants.o
SET(LIBS
        ${UD}/mathfunc.c
        ${UD}/mathfunc.h
        ${UD}/util.c
        ${UD}/util.h
        ${UD}/primes.c
        ${UD}/primes.h
        ${UD}/bitset.c
        ${UD}/bitset.h
        ${UD}/bag.c
        ${UD}/bag.h
        ${UD}/Network.cpp
        ${UD}/Network.h
        UbManager.cpp
        UbManager.h
        constants.c
        constants.h
        )


add_executable(exec_sddp ${LIBS}
        ${UD}/SolveAssignmentProblem.cpp
        ${UD}/SolveAssignmentProblem.h
        sddp/Parameters.cpp
        sddp/CostFunctionSddp.cpp
        sddp/LoadSddp.cpp
        sddp/InsRmvMethodSddp.cpp
        sddp/InsRmvMethodUlmer.cpp
        sddp/InsRmvMethodTsp.cpp
        sddp/Scenario.h
        sddp/Scenarios.cpp
        sddp/Decisions.cpp
        sddp/Solver.cpp
        sddp/SolverUlmer.cpp
        sddp/DecisionMultiSet.cpp
        sddp/DecisionsGroup.cpp
        sddp/BranchRegretSimulation.cpp
        sddp/DynSimulation.cpp
        sddp/DynSimulationUlmer.cpp
        sddp/StaticSimulation.cpp
        sddp/ConsensusSimulation.cpp
        sddp/ExchangeSddp.cpp
        sddp/RelocateSddp.cpp
        sddp/Solver.cpp
        sddp/Report.cpp
        sddp/main_sddp.cpp
        sddp/ProgressiveSimulation.cpp
        sddp/ProgressiveSimulation.h)