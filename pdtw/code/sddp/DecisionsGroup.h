
/*
	DecisionsGroup is a set of sets of decisions
	Each set contains several decisions to be taken all together
*/


#ifndef DECISIONS_GROUP_H
#define DECISIONS_GROUP_H
#include "Decisions.h"
#include <vector>

class DecisionsGroup
{
public:

	void Add(Decisions & dd) {decisions_grp.push_back(dd);}
	int GetCount() { return (int)decisions_grp.size(); }
	Decisions * Get(int i) { return &decisions_grp[i]; }
	void Clear() { decisions_grp.clear(); }

	void Show();

private:
	std::vector<Decisions>  decisions_grp;
};

#endif
