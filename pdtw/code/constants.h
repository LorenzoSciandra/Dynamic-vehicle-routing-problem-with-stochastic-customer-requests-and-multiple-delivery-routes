/*
 * Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca
 * If you have improvements, please contact me!
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H
#ifdef __cplusplus
	extern "C" {
#endif

#define NODE_TYPE_CUSTOMER 0x01
#define NODE_TYPE_PICKUP 0x03
#define NODE_TYPE_DROP 0x05
#define NODE_TYPE_START_DEPOT 0x02
#define NODE_TYPE_END_DEPOT 0x04

char * Constants_Get_Node_Type(int type);

#define STATUS_UNDETERMINED 2		//we tried to solve but we couldn't
#define STATUS_SOLVED 1				//it has been solved!
#define STATUS_UNSOLVED 0			//not yet solved
#define STATUS_PACK_TIMEOUT 3		//got a packing timeout
#define STATUS_ROUTING_TIMEOUT 4	//got a timeout from a routing routine
#define STATUS_TIMEOUT 7			//got a timeout
#define STATUS_FEASIBLE 5			//the problem is feasible
#define STATUS_INFEASIBLE 6			//the problem is infeasible

#define CONF_S2DVRP_BOUNDING_S 1
#define CONF_S2DVRP_BOUNDING_S_FRAC 2
#define CONF_S2DVRP_OPT_CUT_PATH 4

#define ALNS_P 6

#define INF_ROUTE_COST 10000
#define UNASSIGNED_COST 100000
#define BIG_DOUBLE 9999999999.9

#define MPDTW_ALPHA 1
#define MPDTW_BETA 1


#ifdef __cplusplus
	}
#endif

#endif
