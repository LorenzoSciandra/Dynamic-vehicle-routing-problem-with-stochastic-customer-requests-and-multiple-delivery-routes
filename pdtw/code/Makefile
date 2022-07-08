
# ---------------------------------------------------------------------
# Compiler selection
# ---------------------------------------------------------------------

CCC = clang++ -std=c++11
CC  = clang
#CCC = g++
#CC = gcc
JAVAC = javac

# ---------------------------------------------------------------------
# Compiler options
# ---------------------------------------------------------------------

LIBPATH = ../../lib/
CVRPSEP = ../../lib/CVRPSEP/
CPLEXDIR = /Users/jf/ILOG

#instruction pour mon ordi
DFLAGS = -O3 -m64
CCOPT = $(CCOPT) -fPIC -fexceptions -DNDEBUG -DIL_STD
CPLEX_FLAGS = 	$(CPLEXDIR)/concert/lib/x86-64_osx/static_pic/libconcert.a $(CPLEXDIR)/cplex/lib/x86-64_osx/static_pic/libcplex.a $(CPLEXDIR)/cplex/lib/x86-64_osx/static_pic/libilocplex.a
CPLEX_COMPILER = -I$(CPLEXDIR)/concert/include -I$(CPLEXDIR)/cplex/include -DIL_STD
CPLEX_LINKER = -lstdc++ -lm -lpthread -framework CoreFoundation -framework IOKit -L$(CPLEXDIR)/cplex/lib/x86-64_osx -L$(CPLEXDIR)/concert/lib/x86-64_osx


#instruction for CIRRELT or Leandro
#DFLAGS = -O3 -m64
#CCOPT = $(CCOPT) -fPIC -fexceptions -DNDEBUG -DIL_STD
#CPLEX_FLAGS = -lilocplex -lconcert -lcplex -lm -lpthread -fPIC -fexceptions -DNDEBUG -DIL_STD
#CPLEX_COMPILER = $(CPLEX_FLAGS)


lib = mathfunc.o util.o primes.o bitset.o bag.o Network.o UbManager.o constants.o

cvrpsep = basegrph.o binpack.o capsep.o cnstrmgr.o compcuts.o compress.o cutbase.o fcapfix.o fcisep.o grsearch.o hpmstar.o htoursep.o memmod.o mstarsep.o mxf.o newhtour.o sort.o strcomb.o strngcmp.o twomatch.o blocks.o brnching.o combsep.o fcits.o intap.o

mpdtw1 = ParametersMpdtw.o CostFunctionMPDTW.o LoadMPDTW.o LoadPDTW_M.o InsRmvMethodMpdtw.o InsRmvMethodMpdtwNearest.o InsRmvMethodMpdtwFarthest.o InsRmvMethodMpdtwSimple.o InsRmvMethodMpdtwSlow.o InsRmvMethodMpdtwRnd.o RelatednessComplex.o

mpdtw1ex = $(mpdtw1) ExactMpdtwGraph.o ExactMpdtwSep.o ExactMpdtwCallBacks.o ExactMpdtw.o ChainLength.o

pdtw1 =  CostFunctionPDTW.o LoadPDTW.o InsRmvMethodPDTW.o InsRmvMethodPDTWSlow.o

hospital1 =  CostFunctionPDTW.o CostFunctionPdtwSoft.o LoadHospital.o InsRmvMethodPDTW.o InsRmvMethodPDTWSlow.o InsRmvMethodPdtwSoft.o

agv1 = CostFunctionPDTW.o LoadAgv.o InsRmvMethodAgv.o

sddp1 = ParametersSddp.o CostFunctionSddp.o LoadSddp.o InsRmvMethodSddp.o InsRmvMethodUlmer.o InsRmvMethodTsp.o Scenario.o Scenarios.o Decisions.o Solver.o SolverUlmer.o DecisionMultiSet.o DecisionsGroup.o BranchRegretSimulation.o DynSimulation.o DynSimulationUlmer.o StaticSimulation.o ConsensusSimulation.o ExchangeSddp.o RelocateSddp.o SolveAssignmentProblem.o Report.o

patient1 = ParametersPatient.o CostFunctionPatient.o InsRmvMethodPatient.o ScenarioPatient.o ScenariosPatient.o DecisionsPatient.o SolverPatient.o DecisionMultiSetPatient.o DecisionsGroupPatient.o DynSimulationPatient.o StaticSimulationPatient.o SolveAssignmentProblem.o

# --------- EXECUTABLES

# --- ALNS pdtw
pdtw: $(lib)  $(pdtw1) main_pdtw.o
	$(CCC) $(DFLAGS) $(lib) $(pdtw1) main_pdtw.o -o exec_pdtw

# ---- ALNS for the hospital problem
hos: $(lib)  $(hospital1) main_hospital.o
	$(CCC) $(DFLAGS) $(lib) $(hospital1) main_hospital.o -o exec_hospital

agv: $(lib)  $(agv1) main_agv.o
	$(CCC) $(DFLAGS) $(lib) $(agv1) main_agv.o -o exec_agv

patient: $(lib)  $(patient1) main_patient.o
	$(CCC) $(DFLAGS) $(lib) $(patient1) main_patient.o -o exec_patient

sddp: $(lib)  $(sddp1) main_sddp.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_sddp.o -o exec_sddp

sddp_h: $(lib)  $(sddp1) main_test_horizon.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_horizon.o -o exec_sddp_h

sddp_a: $(lib)  $(sddp1) main_test_alns.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_alns.o -o exec_sddp_a

sddp_s: $(lib)  $(sddp1) main_test_scenarios.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_scenarios.o -o exec_sddp_s

sddp_oa: $(lib)  $(sddp1) main_test_oa.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_oa.o -o exec_sddp_oa

sddp_rtd: $(lib)  $(sddp1) main_test_rtd.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_rtd.o -o exec_sddp_rtd

sddp_wd: $(lib)  $(sddp1) main_test_wd.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_wd.o -o exec_sddp_wd

sddp_lcmc: $(lib)  $(sddp1) main_test_lcmc.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_lcmc.o -o exec_sddp_lcmc

sddp_wait: $(lib)  $(sddp1) main_test_wait_eval.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_wait_eval.o -o exec_sddp_wait_eval

sddp_u: $(lib)  $(sddp1) main_ulmer.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_ulmer.o -o exec_sddp_u

sddp_hom2: $(lib)  $(sddp1) main_test_hom2.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_hom2.o -o exec_sddp_hom2

sddp_hom2dt1: $(lib)  $(sddp1) main_test_hom2dt1.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_hom2dt1.o -o exec_sddp_hom2dt1

sddp_homx: $(lib)  $(sddp1) main_test_homx.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_homx.o -o exec_sddp_homx

sddp_final: $(lib)  $(sddp1) main_test_final.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_final.o -o exec_sddp_final

sddp_br: $(lib)  $(sddp1) main_test_branch_and_regret.o
	$(CCC) $(DFLAGS) $(lib) $(sddp1) main_test_branch_and_regret.o -o exec_sddp_br


# --- mpdtw
mpdtw_ex: $(lib) $(cvrpsep) $(mpdtw1ex) main_exact.o
	$(CCC) $(DFLAGS) $(lib) $(cvrpsep) $(mpdtw1ex) main_exact.o -o exec_ex $(CPLEX_LINKER) $(CPLEX_FLAGS)

mpdtw_heur: $(lib)  $(mpdtw1) main_heur.o
	$(CCC) $(DFLAGS) $(lib) $(mpdtw1) main_heur.o -o exec_heur

mpdtw_gen: $(lib)  $(mainV) main_gen_instance.o
	$(CCC) $(DFLAGS) $(lib) $(mainV) main_gen_instance.o -o exec_gen

mpdtw_params: $(lib) $(cvrpsep) $(exact) main_params.o
	$(CCC) $(DFLAGS) $(lib) $(cvrpsep) $(exact) main_params.o -o exec_params $(CPLEX_LINKER) $(CPLEX_FLAGS)



# ------- general classes

headers = ProblemDefinition.h Solution.h Move.h CostFunction.h InsRmvMethod.h SequentialInsertion.h RemoveOperator.h RemoveRandom.h RemoveRelated.h Relatedness.h RegretInsertion.h RegretInsertionOperator.h ALNS.h Relocate.h CreateDriver.h ISolutionList.h BestSolutionList.h RemoveScore.h FeasibleRouteList.h WorstRequestRemoval.h Exchange.h SolCompact.h






# ------ pdtw
headersPDTW = ./pdtw/DriverPDTW.h ./pdtw/NodePDTW.h

LoadPDTW.o: ./pdtw/LoadPDTW.h ./pdtw/LoadPDTW.cpp $(headers) $(headersPDTW)
	$(CCC) -c $(DFLAGS) ./pdtw/LoadPDTW.cpp -o LoadPDTW.o

LoadHospital.o: ./pdtw/LoadHospital.h ./pdtw/LoadHospital.cpp $(headers) $(headersPDTW)
	$(CCC) -c $(DFLAGS) ./pdtw/LoadHospital.cpp -o LoadHospital.o

LoadAgv.o: ./pdtw/LoadAgv.h ./pdtw/LoadAgv.cpp $(headers) $(headersPDTW)
	$(CCC) -c $(DFLAGS) ./pdtw/LoadAgv.cpp -o LoadAgv.o

CostFunctionPDTW.o: ./pdtw/CostFunctionPDTW.h ./pdtw/CostFunctionPDTW.cpp $(headers) $(headersPDTW)
	$(CCC) -c $(DFLAGS) ./pdtw/CostFunctionPDTW.cpp -o CostFunctionPDTW.o

CostFunctionPdtwSoft.o: ./pdtw/CostFunctionPdtwSoft.h ./pdtw/CostFunctionPdtwSoft.cpp $(headers) $(headersPDTW)
	$(CCC) -c $(DFLAGS) ./pdtw/CostFunctionPdtwSoft.cpp -o CostFunctionPdtwSoft.o

InsRmvMethodPDTW.o:	./pdtw/InsRmvMethodPDTW.h ./pdtw/InsRmvMethodPDTW.cpp $(headers) $(headerspdtw)
			$(CCC) -c $(DFLAGS) ./pdtw/InsRmvMethodPDTW.cpp -o InsRmvMethodPDTW.o

InsRmvMethodPDTWSlow.o:	./pdtw/InsRmvMethodPDTWSlow.h ./pdtw/InsRmvMethodPDTWSlow.cpp $(headers) $(headerspdtw)
			$(CCC) -c $(DFLAGS) ./pdtw/InsRmvMethodPDTWSlow.cpp -o InsRmvMethodPDTWSlow.o

InsRmvMethodPdtwSoft.o:	./pdtw/InsRmvMethodPdtwSoft.h ./pdtw/InsRmvMethodPdtwSoft.cpp $(headers) $(headerspdtw)
			$(CCC) -c $(DFLAGS) ./pdtw/InsRmvMethodPdtwSoft.cpp -o InsRmvMethodPdtwSoft.o

InsRmvMethodAgv.o:	./pdtw/InsRmvMethodAgv.h ./pdtw/InsRmvMethodAgv.cpp $(headers) $(headerspdtw)
			$(CCC) -c $(DFLAGS) ./pdtw/InsRmvMethodAgv.cpp -o InsRmvMethodAgv.o

main_pdtw.o: ./pdtw/main_pdtw.cpp $(headers) $(headersPDTW)
	$(CCC) -c $(DFLAGS) ./pdtw/main_pdtw.cpp -o main_pdtw.o

main_hospital.o: ./pdtw/main_hospital.cpp $(headers) $(headersPDTW)
	$(CCC) -c $(DFLAGS) ./pdtw/main_hospital.cpp -o main_hospital.o

main_agv.o: ./pdtw/main_agv.cpp $(headers) $(headersPDTW)
	$(CCC) -c $(DFLAGS) ./pdtw/main_agv.cpp -o main_agv.o









# ------ sddp
headersSddp = ./sddp/DriverSddp.h ./sddp/NodeSddp.h ./sddp/RelatednessSddp.h ./sddp/Parameters.h ./sddp/Decision.h ./sddp/Test.h

ParametersSddp.o: ./sddp/Parameters.h ./sddp/Parameters.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/Parameters.cpp -o ParametersSddp.o

LoadSddp.o: ./sddp/LoadSddp.h ./sddp/LoadSddp.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/LoadSddp.cpp -o LoadSddp.o

CostFunctionSddp.o: ./sddp/CostFunctionSddp.h ./sddp/CostFunctionSddp.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/CostFunctionSddp.cpp -o CostFunctionSddp.o

InsRmvMethodSddp.o:	./sddp/InsRmvMethodSddp.h ./sddp/InsRmvMethodSddp.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/InsRmvMethodSddp.cpp -o InsRmvMethodSddp.o

InsRmvMethodUlmer.o:	./sddp/InsRmvMethodUlmer.h ./sddp/InsRmvMethodUlmer.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/InsRmvMethodUlmer.cpp -o InsRmvMethodUlmer.o

InsRmvMethodTsp.o:	./sddp/InsRmvMethodTsp.h ./sddp/InsRmvMethodTsp.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/InsRmvMethodTsp.cpp -o InsRmvMethodTsp.o

Decisions.o:	./sddp/Decisions.h ./sddp/Decisions.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/Decisions.cpp -o Decisions.o

Scenario.o:	./sddp/Scenario.h ./sddp/Scenario.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/Scenario.cpp -o Scenario.o

Scenarios.o:	./sddp/Scenarios.h ./sddp/Scenarios.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/Scenarios.cpp -o Scenarios.o

Solver.o:	./sddp/Solver.h ./sddp/Solver.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/Solver.cpp -o Solver.o

SolverUlmer.o:	./sddp/SolverUlmer.h ./sddp/SolverUlmer.cpp $(headers) $(headersSddp) ./sddp/Opt2.h
			$(CCC) -c $(DFLAGS) ./sddp/SolverUlmer.cpp -o SolverUlmer.o

DecisionMultiSet.o:	./sddp/DecisionMultiSet.h ./sddp/DecisionMultiSet.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/DecisionMultiSet.cpp -o DecisionMultiSet.o

DecisionsGroup.o:	./sddp/DecisionsGroup.h ./sddp/DecisionsGroup.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/DecisionsGroup.cpp -o DecisionsGroup.o

ExchangeSddp.o:	./sddp/ExchangeSddp.h ./sddp/ExchangeSddp.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/ExchangeSddp.cpp -o ExchangeSddp.o

RelocateSddp.o:	./sddp/RelocateSddp.h ./sddp/RelocateSddp.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/RelocateSddp.cpp -o RelocateSddp.o

BranchRegretSimulation.o:	./sddp/BranchRegretSimulation.h ./sddp/BranchRegretSimulation.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/BranchRegretSimulation.cpp -o BranchRegretSimulation.o

DynSimulation.o:	./sddp/DynSimulation.h ./sddp/DynSimulation.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/DynSimulation.cpp -o DynSimulation.o

DynSimulationUlmer.o:	./sddp/DynSimulationUlmer.h ./sddp/DynSimulationUlmer.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/DynSimulationUlmer.cpp -o DynSimulationUlmer.o

StaticSimulation.o:	./sddp/StaticSimulation.h ./sddp/StaticSimulation.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/StaticSimulation.cpp -o StaticSimulation.o

ConsensusSimulation.o:	./sddp/ConsensusSimulation.h ./sddp/ConsensusSimulation.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/ConsensusSimulation.cpp -o ConsensusSimulation.o

Report.o:	./sddp/Report.h ./sddp/Report.cpp $(headers) $(headersSddp)
			$(CCC) -c $(DFLAGS) ./sddp/Report.cpp -o Report.o

main_sddp.o: ./sddp/main_sddp.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_sddp.cpp -o main_sddp.o

main_test_horizon.o: ./sddp/main_test_horizon.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_horizon.cpp -o main_test_horizon.o

main_test_alns.o: ./sddp/main_test_alns.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_alns.cpp -o main_test_alns.o

main_test_scenarios.o: ./sddp/main_test_scenarios.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_scenarios.cpp -o main_test_scenarios.o

main_test_oa.o: ./sddp/main_test_oa.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_oa.cpp -o main_test_oa.o

main_test_rtd.o: ./sddp/main_test_rtd.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_rtd.cpp -o main_test_rtd.o

main_test_wd.o: ./sddp/main_test_wd.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_wd.cpp -o main_test_wd.o

main_test_lcmc.o: ./sddp/main_test_lcmc.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_lcmc.cpp -o main_test_lcmc.o

main_test_wait_eval.o: ./sddp/main_test_wait_eval.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_wait_eval.cpp -o main_test_wait_eval.o

main_ulmer.o: ./sddp/main_ulmer.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_ulmer.cpp -o main_ulmer.o

main_test_hom2.o: ./sddp/main_test_hom2.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_hom2.cpp -o main_test_hom2.o

main_test_hom2dt1.o: ./sddp/main_test_hom2dt1.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_hom2dt1.cpp -o main_test_hom2dt1.o

main_test_homx.o: ./sddp/main_test_homx.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_homx.cpp -o main_test_homx.o

main_test_final.o: ./sddp/main_test_final.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_final.cpp -o main_test_final.o

main_test_branch_and_regret.o: ./sddp/main_test_branch_and_regret.cpp $(headers) $(headersSddp)
	$(CCC) -c $(DFLAGS) ./sddp/main_test_branch_and_regret.cpp -o main_test_branch_and_regret.o









# ------ patient
headersPatient = ./patient/DriverPatient.h ./patient/NodePatient.h ./patient/RelatednessPatient.h ./patient/Parameters.h ./patient/Decision.h

ParametersPatient.o: ./patient/Parameters.h ./patient/Parameters.cpp $(headers) $(headersPatient)
	$(CCC) -c $(DFLAGS) ./patient/Parameters.cpp -o ParametersPatient.o

CostFunctionPatient.o: ./patient/CostFunctionPatient.h ./patient/CostFunctionPatient.cpp $(headers) $(headersPatient)
	$(CCC) -c $(DFLAGS) ./patient/CostFunctionPatient.cpp -o CostFunctionPatient.o

InsRmvMethodPatient.o:	./patient/InsRmvMethodPatient.h ./patient/InsRmvMethodPatient.cpp $(headers) $(headersPatient)
			$(CCC) -c $(DFLAGS) ./patient/InsRmvMethodPatient.cpp -o InsRmvMethodPatient.o

DecisionsPatient.o:	./patient/Decisions.h ./patient/Decisions.cpp $(headers) $(headersPatient)
			$(CCC) -c $(DFLAGS) ./patient/Decisions.cpp -o DecisionsPatient.o

ScenarioPatient.o:	./patient/Scenario.h ./patient/Scenario.cpp $(headers) $(headersPatient)
			$(CCC) -c $(DFLAGS) ./patient/Scenario.cpp -o ScenarioPatient.o

ScenariosPatient.o:	./patient/Scenarios.h ./patient/Scenarios.cpp $(headers) $(headersPatient)
			$(CCC) -c $(DFLAGS) ./patient/Scenarios.cpp -o ScenariosPatient.o

SolverPatient.o:	./patient/Solver.h ./patient/Solver.cpp $(headers) $(headersPatient)
			$(CCC) -c $(DFLAGS) ./patient/Solver.cpp -o SolverPatient.o

DecisionMultiSetPatient.o:	./patient/DecisionMultiSet.h ./patient/DecisionMultiSet.cpp $(headers) $(headersPatient)
			$(CCC) -c $(DFLAGS) ./patient/DecisionMultiSet.cpp -o DecisionMultiSetPatient.o

DecisionsGroupPatient.o:	./patient/DecisionsGroup.h ./patient/DecisionsGroup.cpp $(headers) $(headersPatient)
			$(CCC) -c $(DFLAGS) ./patient/DecisionsGroup.cpp -o DecisionsGroupPatient.o

DynSimulationPatient.o:	./patient/DynSimulation.h ./patient/DynSimulation.cpp $(headers) $(headersPatient)
			$(CCC) -c $(DFLAGS) ./patient/DynSimulation.cpp -o DynSimulationPatient.o

StaticSimulationPatient.o:	./patient/StaticSimulation.h ./patient/StaticSimulation.cpp $(headers) $(headersPatient)
			$(CCC) -c $(DFLAGS) ./patient/StaticSimulation.cpp -o StaticSimulationPatient.o

main_patient.o: ./patient/main_patient.cpp $(headers) $(headersPatient)
	$(CCC) -c $(DFLAGS) ./patient/main_patient.cpp -o main_patient.o


























# ------ mpdtw
headersMPDTW = ./mpdtw/DriverMPDTW.h ./mpdtw/NodeMPDTW.h ./mpdtw/InsRmvMethodMpdtw.h ./mpdtw/InsRmvMethodMpdtw.cpp
headersExact = $(headersMPDTW) ./mpdtw/ExactMpdtw.h ./mpdtw/ExactMpdtw.cpp ./mpdtw/ExactMpdtwGraph.h ./mpdtw/ExactMpdtwGraph.cpp

main_heur.o: ./mpdtw/main_heur.cpp $(headers) $(headersMPDTW)
	$(CCC) -c $(DFLAGS) ./mpdtw/main_heur.cpp -o main_heur.o

main_params.o: ./mpdtw/main_params.cpp $(headers) $(headersMPDTW)
	$(CCC) -c $(DFLAGS) ./mpdtw/main_params.cpp -o main_params.o $(CPLEX_COMPILER)

main_exact.o: ./mpdtw/main_exact.cpp $(headers) $(headersMPDTW)
	$(CCC) -c $(DFLAGS) ./mpdtw/main_exact.cpp -o main_exact.o  $(CPLEX_COMPILER)

main_gen_instance.o: ./mpdtw/main_gen_instance.cpp $(headers) $(headersMPDTW)
	$(CCC) -c $(DFLAGS) ./mpdtw/main_gen_instance.cpp -o main_gen_instance.o

LoadMPDTW.o: ./mpdtw/LoadMPDTW.h ./mpdtw/LoadMPDTW.cpp $(headers) $(headersMPDTW)
	$(CCC) -c $(DFLAGS) ./mpdtw/LoadMPDTW.cpp -o LoadMPDTW.o

LoadPDTW_M.o: ./mpdtw/LoadPDTW.h ./mpdtw/LoadPDTW.cpp $(headers) $(headersMPDTW)
	$(CCC) -c $(DFLAGS) ./mpdtw/LoadPDTW.cpp -o LoadPDTW_M.o

CostFunctionMPDTW.o: ./mpdtw/CostFunctionMPDTW.h ./mpdtw/CostFunctionMPDTW.cpp $(headers) $(headersMPDTW)
	$(CCC) -c $(DFLAGS) ./mpdtw/CostFunctionMPDTW.cpp -o CostFunctionMPDTW.o

InsRmvMethodMpdtw.o:	./mpdtw/InsRmvMethodMpdtw.h ./mpdtw/InsRmvMethodMpdtw.cpp $(headers) $(headersMPDTW)
			$(CCC) -c $(DFLAGS) ./mpdtw/InsRmvMethodMpdtw.cpp -o InsRmvMethodMpdtw.o

InsRmvMethodMpdtwSlow.o:	./mpdtw/InsRmvMethodMpdtwSlow.h ./mpdtw/InsRmvMethodMpdtwSlow.cpp $(headers) $(headersMPDTW)
			$(CCC) -c $(DFLAGS) ./mpdtw/InsRmvMethodMpdtwSlow.cpp -o InsRmvMethodMpdtwSlow.o

InsRmvMethodMpdtwRnd.o:	./mpdtw/InsRmvMethodMpdtwRnd.h ./mpdtw/InsRmvMethodMpdtwRnd.cpp $(headers) $(headersMPDTW2)
			$(CCC) -c $(DFLAGS) ./mpdtw/InsRmvMethodMpdtwRnd.cpp -o InsRmvMethodMpdtwRnd.o

InsRmvMethodMpdtwNearest.o:	./mpdtw/InsRmvMethodMpdtwNearest.h ./mpdtw/InsRmvMethodMpdtwNearest.cpp $(headers) $(headersMPDTWNearest)
			$(CCC) -c $(DFLAGS) ./mpdtw/InsRmvMethodMpdtwNearest.cpp -o InsRmvMethodMpdtwNearest.o

InsRmvMethodMpdtwFarthest.o:	./mpdtw/InsRmvMethodMpdtwFarthest.h ./mpdtw/InsRmvMethodMpdtwFarthest.cpp $(headers) $(headersMPDTWFarthest)
			$(CCC) -c $(DFLAGS) ./mpdtw/InsRmvMethodMpdtwFarthest.cpp -o InsRmvMethodMpdtwFarthest.o

InsRmvMethodMpdtwSimple.o:	./mpdtw/InsRmvMethodMpdtwSimple.h ./mpdtw/InsRmvMethodMpdtwSimple.cpp $(headers) $(headersMPDTWSimple)
			$(CCC) -c $(DFLAGS) ./mpdtw/InsRmvMethodMpdtwSimple.cpp -o InsRmvMethodMpdtwSimple.o

RelatednessComplex.o:	./mpdtw/RelatednessComplex.h ./mpdtw/RelatednessComplex.cpp $(headers) $(headersMPDTWSimple)
			$(CCC) -c $(DFLAGS) ./mpdtw/RelatednessComplex.cpp -o RelatednessComplex.o

ExactMpdtwSep.o:		./mpdtw/ExactMpdtwSep.h ./mpdtw/ExactMpdtwSep.cpp $(headers) $(headersMPDTW)
			$(CCC) -c $(DFLAGS) ./mpdtw/ExactMpdtwSep.cpp -o ExactMpdtwSep.o $(CPLEX_COMPILER)

ExactMpdtwCallBacks.o:		./mpdtw/ExactMpdtwCallBacks.h ./mpdtw/ExactMpdtwCallBacks.cpp $(headers) $(headersMPDTW)
			$(CCC) -c $(DFLAGS) ./mpdtw/ExactMpdtwCallBacks.cpp -o ExactMpdtwCallBacks.o $(CPLEX_COMPILER)

ExactMpdtw.o:		./mpdtw/ExactMpdtw.h ./mpdtw/ExactMpdtw.cpp $(headers) $(headersMPDTW)
			$(CCC) -c $(DFLAGS) ./mpdtw/ExactMpdtw.cpp -o ExactMpdtw.o $(CPLEX_COMPILER)

ExactMpdtwGraph.o:		./mpdtw/ExactMpdtwGraph.h ./mpdtw/ExactMpdtwGraph.cpp $(headers) $(headersMPDTW)
			$(CCC) -c $(DFLAGS) ./mpdtw/ExactMpdtwGraph.cpp -o ExactMpdtwGraph.o $(CPLEX_COMPILER)

ChainLength.o:		./mpdtw/ChainLength.h ./mpdtw/ChainLength.cpp $(headers) $(headersMPDTW)
			$(CCC) -c $(DFLAGS) ./mpdtw/ChainLength.cpp -o ChainLength.o $(CPLEX_COMPILER)

ParametersMpdtw.o  :	./mpdtw/ParametersMpdtw.h ./mpdtw/ParametersMpdtw.cpp
			$(CCC) -c $(DFLAGS) ./mpdtw/ParametersMpdtw.cpp -o ./mpdtw/ParametersMpdtw.o

#  ------- lib

constants.o  :	constants.h constants.c
			$(CC) -c $(DFLAGS) constants.c -o constants.o

bag.o  :	$(LIBPATH)bag.h $(LIBPATH)bag.c
			$(CC) -c $(DFLAGS) $(LIBPATH)bag.c -o bag.o

primes.o  :	$(LIBPATH)primes.h $(LIBPATH)primes.c
			$(CC) -c $(DFLAGS) $(LIBPATH)primes.c -o primes.o

bitset.o  :	$(LIBPATH)bitset.h $(LIBPATH)bitset.c
			$(CC) -c $(DFLAGS) $(LIBPATH)bitset.c -o bitset.o

util.o  :	$(LIBPATH)util.h $(LIBPATH)util.c
			$(CC) -c $(DFLAGS) $(LIBPATH)util.c -o util.o

mathfunc.o  :	$(LIBPATH)mathfunc.h $(LIBPATH)mathfunc.c
			$(CC) -c $(DFLAGS) $(LIBPATH)mathfunc.c -o mathfunc.o

Network.o  :	$(LIBPATH)Network.h $(LIBPATH)Network.cpp
			$(CCC) -c $(DFLAGS) $(LIBPATH)Network.cpp -o Network.o

UbManager.o  :	UbManager.h UbManager.cpp
			$(CCC) -c $(DFLAGS) UbManager.cpp -o UbManager.o

SolveAssignmentProblem.o  :	$(LIBPATH)SolveAssignmentProblem.h $(LIBPATH)SolveAssignmentProblem.cpp
			$(CCC) -c $(DFLAGS) $(LIBPATH)SolveAssignmentProblem.cpp -o SolveAssignmentProblem.o

#  ------- CVRPSEP
basegrph.o : $(CVRPSEP)basegrph.h $(CVRPSEP)basegrph.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)basegrph.cpp -o basegrph.o

binpack.o : $(CVRPSEP)binpack.h $(CVRPSEP)binpack.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)binpack.cpp -o binpack.o

capsep.o : $(CVRPSEP)capsep.h $(CVRPSEP)capsep.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)capsep.cpp -o capsep.o

cnstrmgr.o : $(CVRPSEP)cnstrmgr.h $(CVRPSEP)cnstrmgr.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)cnstrmgr.cpp -o cnstrmgr.o

compcuts.o : $(CVRPSEP)compcuts.h $(CVRPSEP)compcuts.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)compcuts.cpp -o compcuts.o

compress.o : $(CVRPSEP)compress.h $(CVRPSEP)compress.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)compress.cpp -o compress.o

cutbase.o : $(CVRPSEP)cutbase.h $(CVRPSEP)cutbase.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)cutbase.cpp -o cutbase.o

fcapfix.o : $(CVRPSEP)fcapfix.h $(CVRPSEP)fcapfix.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)fcapfix.cpp -o fcapfix.o

fcisep.o : $(CVRPSEP)fcisep.h $(CVRPSEP)fcisep.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)fcisep.cpp -o fcisep.o

grsearch.o : $(CVRPSEP)grsearch.h $(CVRPSEP)grsearch.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)grsearch.cpp -o grsearch.o

hpmstar.o : $(CVRPSEP)hpmstar.h $(CVRPSEP)hpmstar.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)hpmstar.cpp -o hpmstar.o

htoursep.o : $(CVRPSEP)htoursep.h $(CVRPSEP)htoursep.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)htoursep.cpp -o htoursep.o

memmod.o : $(CVRPSEP)memmod.h $(CVRPSEP)memmod.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)memmod.cpp -o memmod.o

mstarsep.o : $(CVRPSEP)mstarsep.h $(CVRPSEP)mstarsep.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)mstarsep.cpp -o mstarsep.o

mxf.o : $(CVRPSEP)mxf.h $(CVRPSEP)mxf.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)mxf.cpp -o mxf.o

newhtour.o : $(CVRPSEP)newhtour.h $(CVRPSEP)newhtour.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)newhtour.cpp -o newhtour.o

sort.o : $(CVRPSEP)sort.h $(CVRPSEP)sort.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)sort.cpp -o sort.o

strcomb.o : $(CVRPSEP)strcomb.h $(CVRPSEP)strcomb.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)strcomb.cpp -o strcomb.o

strngcmp.o : $(CVRPSEP)strngcmp.h $(CVRPSEP)strngcmp.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)strngcmp.cpp -o strngcmp.o

twomatch.o : $(CVRPSEP)twomatch.h $(CVRPSEP)twomatch.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)twomatch.cpp -o twomatch.o

blocks.o : $(CVRPSEP)blocks.h $(CVRPSEP)blocks.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)blocks.cpp -o blocks.o

brnching.o : $(CVRPSEP)brnching.h $(CVRPSEP)brnching.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)brnching.cpp -o brnching.o

combsep.o : $(CVRPSEP)combsep.h $(CVRPSEP)combsep.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)combsep.cpp -o combsep.o

fcits.o : $(CVRPSEP)fcits.h $(CVRPSEP)fcits.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)fcits.cpp -o fcits.o

glmsep.o : $(CVRPSEP)glmsep.h $(CVRPSEP)glmsep.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)glmsep.cpp -o glmsep.o

intap.o : $(CVRPSEP)intap.h $(CVRPSEP)intap.cpp
			$(CCC) -c $(DFLAGS) $(CVRPSEP)intap.cpp -o intap.o

# ------------------------------------------------------------

clean :
	/bin/rm -rf *.o *~ exec_*
	/bin/rm -rf $(C_EX) $(CPP_EX)
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log

# ------------------------------------------------------------
# DO NOT DELETE
