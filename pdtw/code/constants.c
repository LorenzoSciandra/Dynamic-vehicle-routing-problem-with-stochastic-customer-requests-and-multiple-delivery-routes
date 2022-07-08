#include "constants.h"

char * Constants_Get_Node_Type(int type)
{
	if(type == NODE_TYPE_CUSTOMER) return (char*)"cust";
	else if(type == NODE_TYPE_PICKUP) return (char*)"pick";
	else if(type == NODE_TYPE_DROP) return (char*)"drop";
	else if(type == NODE_TYPE_START_DEPOT) return (char*)"stad";
	else if(type == NODE_TYPE_END_DEPOT) return (char*)"endd";
	else return (char*)"unkn";
}
