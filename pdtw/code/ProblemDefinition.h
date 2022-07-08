/*
 * Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
*/

#ifndef PROBLEM_DEF_
#define PROBLEM_DEF_

#include <stddef.h>
#include <vector>
#include <cstring>
#include "constants.h"
#include "../../lib/primes.h"
#include "Request.h"
#include <stdio.h>
#include <stdlib.h>

template <class NodeT, class DriverT>
class Prob
{
	public:
    //Constructor defined using member initialization as:
	Prob(): _nodes(0),_customers(0),_drivers(0),_requests(0), _nodes_request(0),_distances(NULL), _times(NULL), _dimension(0),_upper_bound(9999999999.9),_wanted_driver_count(0),_driver_lb(0),_driver_ub(0), nb_requests(0)
	{
		_nodes.reserve(2000);		//not so clean....
		_customers.reserve(2000);
		_drivers.reserve(200);
		_requests.reserve(2000);
		_nodes_request.reserve(2000);
		memset(_problemName, 0, sizeof(_problemName));
		memset(_shortName, 0, sizeof(_shortName));
		memset(_path, 0, sizeof(_path));
	}
	~Prob()
	{
		if(_distances != NULL)
		{
			for(int i = 0 ; i < _dimension ; i++)
				delete [] _distances[i];
			delete [] _distances;
		}
		if(_times != NULL && _distances != _times)
		{
			for(int i = 0 ; i < _dimension ; i++)
				delete [] _times[i];
			delete [] _times;
		}
		_times = NULL;
		_distances = NULL;
	}
	void Reserve()
	{
		_nodes.reserve(2000);		//not so clean....
		_customers.reserve(2000);
		_drivers.reserve(200);
		_requests.reserve(2000);
		_nodes_request.reserve(2000);
	}
	void Reserve(int nb_customers, int nb_drivers)
	{
		_nodes.reserve(nb_customers + nb_drivers*2);
		_customers.reserve(nb_customers + nb_drivers*2);
		_drivers.reserve(nb_drivers);
		_requests.reserve(nb_customers + nb_drivers*2);
		_nodes_request.reserve(nb_customers + nb_drivers*2);
	}


	void AddNode(NodeT & n)
	{
		_nodes.push_back(n);
		if(n.IsCustomer())
			_customers.push_back(n.id);
	}
	void AddRequest(Request<NodeT> & r)
	{
		_requests.push_back(r);
		for (int i=0 ; i < r.GetNodeCount() ; i++)
			_nodes_request[r.GetNode(i)->id] = r.id;
		nb_requests ++;
	}
	int GetCustomerCount(){ return (int)_customers.size();}
	int GetNodeCount(){ return (int)_nodes.size();}
	int GetRequestCount() { return (int) _requests.size();}
	NodeT* GetCustomer(int i){ return &_nodes[_customers[i]]; }
	NodeT* GetNode(int i){ return &_nodes[i]; }
	Request<NodeT> * GetRequest(int i) { return &_requests[i]; }
	Request<NodeT> * GetRequest(NodeT* n) { return  &_requests[ _nodes_request[n->id] ]; }
	NodeT* GetRequestParent(int i){return _requests[i].GetParent();}
	void SetRequest(NodeT* n, Request<NodeT> * r){_nodes_request[n->id] = r;}
	void NodeRequestResize(){_nodes_request.resize(_nodes.size());}
	void AddDriver(DriverT & d){ _drivers.push_back(d);}
	int GetDriverCount(){ return (int)_drivers.size();}
	DriverT* GetDriver(int i){ return &_drivers[i];}


	void SetMaxtrices(double ** d, double ** t, int dim) { _distances = d; _times = t; _dimension = dim;}
	double ** GetDistances(){ return _distances;}
	double ** GetTimes(){ return _times;}
	double GetTime(NodeT * i, NodeT * j){ return _times[i->distID][j->distID];}
	double GetTime(int i, int j){ return _times[i][j];}
	double GetDistance(NodeT * i, NodeT * j){ return _distances[i->distID][j->distID];}
	double GetDist(NodeT * i, NodeT * j){ return _distances[i->distID][j->distID];}
	double GetDist(int i, int j){ return _distances[i][j];}

	void ShowMatrices()
	{
		printf("Distance dim:%d\n", _dimension);
		for(int i=0;i<_dimension;i++)
		{
			for(int j=0;j<_dimension;j++)
				printf("%.1lf ", _distances[i][j]);
			printf("\n");
		}
		printf("Time dim:%d\n", _dimension);
		for(int i=0;i<_dimension;i++)
		{
			for(int j=0;j<_dimension;j++)
				printf("%.1lf ", _times[i][j]);
			printf("\n");
		}
	}

	int GetDimension(){return _dimension;}
	void TruncateMatrices()
	{
		for(int i=0;i<_dimension;i++)
			for(int j=0;j<_dimension;j++)
			{
				_distances[i][j] = (int)_distances[i][j];
				_times[i][j] = (int)_times[i][j];
			}
	}

	static unsigned long long GetCustomerSetKey(std::vector<NodeT*> & nodes)
	{
		unsigned long long l = 1;
		for(size_t k=0;k<nodes.size();k++)
		{
			if(nodes[k]->type == NODE_TYPE_CUSTOMER)
				l *= prime_get_ith(nodes[k]->id);
		}
		return l;
	}

	static void GetIdListNoDepot(std::vector<NodeT*> & nodes, std::vector<int> & ids)
	{
		ids.clear();
		for(size_t k=0;k<nodes.size();k++)
			if(nodes[k]->type == NODE_TYPE_CUSTOMER)
				ids.push_back(nodes[k]->id);
	}

	NodeT* GetNodeByOriginID(int id)
	{
		for(size_t i=0;i<_nodes.size();i++)
			if(_nodes[i].origin_id == id)
				return &(_nodes[i]);
		return NULL;
	}

	char * GetProblemName(){return _problemName;}
	void SetProblemName(char * name)
	{
		strncpy(_problemName,name, sizeof(_problemName));
		int i,last = -1;
		int len = strlen(name);
		for(i=0;i<len;i++)
			if(name[i] == '/')
				last = i;
		for(i=last+1;i<len;i++)
			_shortName[i-last-1] = name[i];
		_shortName[len - last - 1] = '\0';
	}

	char * GetShortName(){return _shortName;}
	void SetShortName(char * name){_shortName = name;}

	char * GetProblemPath(){return _path;}
	void SetProblemPath(char * name){_path = name;}

	double GetUpperBound(){return _upper_bound;}
	void SetUpperBound(double ub){_upper_bound = ub;}

	int GetWantedDriverCount(){return _wanted_driver_count;}
	void SetWantedDriverCount(int ub){_wanted_driver_count = ub;}

	int GetDriverCountLB(){return _driver_lb;}
	void SetDriverCountLB(int lb){_driver_lb = lb;}

	int GetDriverCountUB(){return _driver_ub;}
	void SetDriverCountUB(int ub){_driver_ub = ub;}

	//create a problem using the first driver and the set of nodes
	void CopyImportantDataTo(Prob<NodeT,DriverT> * pr)
	{
		pr->_distances = _distances; //pr->_distances equivalent to (*pr)._distances.  it gets the member called _distances from the class that pr points to.
		pr->_times = _times;
		pr->_dimension = _dimension;
		pr->_problemName = _problemName;
		pr->_shortName = _shortName;
		pr->_path = _path;
		pr->_upper_bound = _upper_bound;
		pr->_wanted_driver_count = _wanted_driver_count;
		pr->_driver_lb = _driver_lb;
		pr->_driver_ub = _driver_ub;
	}

	void Show()
	{
		for(size_t i=0;i<_nodes.size();i++)
			_nodes[i].Show();
		for(size_t i=0;i<_drivers.size();i++)
			_drivers[i].Show();
	}

	private:
	std::vector<NodeT> _nodes;
	std::vector<int> _customers;
	std::vector<DriverT> _drivers;
	std::vector<Request<NodeT> > _requests;
	std::vector<int> _nodes_request;

	double ** _distances;
	double ** _times;
	int _dimension;

	char _problemName[128];
	char _shortName[128];
	char _path[128];

	double _upper_bound;
	int _wanted_driver_count;	//number of wanted drivers
	int _driver_lb;				//lower bound on the number of required drivers
	int _driver_ub;				//upper bound on the number of required drivers
	int nb_requests;
};

#endif
