
#ifndef UB_MANAGER_H
#define UB_MANAGER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <string.h>

/*
	Class that load a list of upper bounds for different problems
	
	Just pass the problem name (or path with name)
	to get the upper bound of the problem
*/

class UbManager
{
	public:
		UbManager(){}
		~UbManager(){}
	
	
		void Load(char * filename, bool has_vehicle_count);
		double GetUpperBound(char * instance);
		int GetVehicleCount(char * instance);
	
	private:
		std::map<std::string, double> _bounds;
		std::map<std::string, int> _vehicles;
};




#endif
