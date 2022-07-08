/*
 * Copyright Jean-Francois Cote 2012
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
 *
*/

#ifndef LOAD_SDDP
#define LOAD_SDDP

#include "../ProblemDefinition.h"

#include "NodeSddp.h"
#include "DriverSddp.h"


class LoadSddp
{
	public:
		void Load(Prob<Node,Driver> & pr, char * filename);

};

#endif
