
#include "UbManager.h"
#include <map>
#include <string>

void UbManager::Load(char * filename,bool has_vehicle_count)
{
	FILE * f = fopen(filename, "r");

	if(f == NULL)
	{
		printf("UbManager::Load\nError at the opening of the file:%s\n", filename);
		return;
	}

	int nb;
	fscanf(f, "%d\n",&nb);


	for(int i=0;i<nb;i++)
	{
		char name[40];
		double bound;
		int vehicle_count = 0;

		if(has_vehicle_count)
			fscanf(f,"%s %lf %d\n", name, &bound, &vehicle_count);
		else
			fscanf(f,"%s %lf\n", name, &bound);
		//printf("%s bound:%lf vc:%d\n",name,bound,vehicle_count);
		std::string st(name);

		_bounds[st] = bound;

		if(has_vehicle_count) _vehicles[st] = vehicle_count;
	}
}

double UbManager::GetUpperBound(char * ins)
{
	int len = strlen(ins);
	char name[40]; bzero(name,40);
	int i,p1 = -1,p2 = -1;
	//check the position of the last \ and the .

	for(i=0;i<len;i++)
	{
		if(ins[i] == '/') p1 = i;
		if(ins[i] == '.') p2 = i;
	}

	for(i=p1+1;i<p2;i++)
		name[i-p1-1] = ins[i];
	name[i] = '\0';

	std::string st(name);
	double bd = _bounds[st];

	//printf("UbManager::GetUpperBound:%s\tname:%s bound:%lf\n", ins,name,bd);
	//printf("Name:%s bound:%lf\n",name, bd);
	if(bd <= 1.0) return 999999999.9;
	else return bd;
}

int UbManager::GetVehicleCount(char * ins)
{
	int len = strlen(ins);
	char name[40]; bzero(name,40);
	int i,p1 = -1,p2 = -1;
	//check the position of the last \ and the .

	for(i=0;i<len;i++)
	{
		if(ins[i] == '/') p1 = i;
		if(ins[i] == '.') p2 = i;
	}

	for(i=p1+1;i<p2;i++)
		name[i-p1-1] = ins[i];
	name[i] = '\0';

	std::string st(name);
	int bd = _vehicles[st];

	//printf("UbManager::GetUpperBound:%s\tname:%s bound:%lf\n", ins,name,bd);
	//printf("Name:%s bound:%lf\n",name, bd);
	if(bd <= 0) return 9999999;
	else return bd;
}
