

#ifndef REQUEST
#define REQUEST

#include <stdio.h>
#include <stdlib.h>
#include <vector>

template<class NodeT>
class Request {
public:
    int id;

    NodeT *GetParent() { return _list[0]; }

    NodeT *GetNode(int i) { return _list[i]; }

    int GetNodeCount() { return _list.size(); }

    void AddNode(NodeT *n) {
        _list.push_back(n);
        if (n->type == NODE_TYPE_PICKUP)
            _pickups.push_back(n);
        else if (n->type == NODE_TYPE_DROP)
            _drops.push_back(n);
        else {
            printf("Error in Request AddNode type:%d\n", (int) n->type);
            exit(1);
        }
    }

    int GetPickupNodeCount() { return (int) _pickups.size(); }

    NodeT *GetPickupNode(int i) { return _pickups[i]; }

    void GetPickupNodes(std::vector<NodeT *> &v) { v = _pickups; }

    int GetDropNodeCount() { return (int) _drops.size(); }

    NodeT *GetDropNode(int i) { return _drops[i]; }

    void GetDropNode(std::vector<NodeT *> &v) { v = _drops; }

    void Show() {
        for (int j = 0; j < _pickups.size(); j++)
            _pickups[j]->Show();
        for (int j = 0; j < _drops.size(); j++)
            _drops[j]->Show();
    }

    void Clear() {
        _list.clear();
        _pickups.clear();
        _drops.clear();
    }

private:
    std::vector<NodeT *> _list;
    std::vector<NodeT *> _pickups;
    std::vector<NodeT *> _drops;
};

#endif
