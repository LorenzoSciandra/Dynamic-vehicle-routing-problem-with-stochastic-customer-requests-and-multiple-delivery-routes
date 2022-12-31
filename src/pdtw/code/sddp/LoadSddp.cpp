
#include "LoadSddp.h"
#include <math.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "../constants.h"


void LoadSddp::Load(Prob<Node, Driver> &pr, char *filename) {
    pr = Prob<Node, Driver>();

    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        printf("Error at the opening of the file:%s\n", filename);
        exit(1);
    }

    int nb_vehicles, cap, speed;

    int r11 = fscanf(f, "%d %d %d\n", &nb_vehicles, &cap, &speed);


    std::vector<float> vec_x(0);
    std::vector<float> vec_y(0);

    int node_id, x, y, dmd, tw_start, tw_end, serv_time, pick_node_id, del_node_id, nb_items, sum_dmd, k;

    //read depot node
    r11 = fscanf(f, "%d %d %d %d %d %d %d %d %d\n", &node_id, &x, &y, &dmd, &tw_start, &tw_end, &serv_time,
                 &pick_node_id, &del_node_id);
    Node dep;
    dep.origin_id = 0;
    dep.x = x;
    dep.y = y;
    dep.demand = dmd;
    dep.tw_start = tw_start;
    dep.tw_end = tw_end;
    dep.serv_time = serv_time;
    dep.type = NODE_TYPE_START_DEPOT;

    dep.pick_node_id = pick_node_id - 1;
    dep.del_node_id = del_node_id - 1;

    vec_x.push_back(x);
    vec_y.push_back(y);


    //read pick up and delivery nodes
    nb_items = 0;

    while (!feof(f)) {
        r11 = fscanf(f, "%d %d %d %d %d %d %d %d %d\n", &node_id, &x, &y, &dmd, &tw_start, &tw_end, &serv_time,
                     &pick_node_id, &del_node_id);

        nb_items++;
        Node n;
        n.origin_id = node_id;
        n.id = node_id - 1;
        n.x = x;
        n.y = y;
        n.demand = dmd;
        n.tw_start = tw_start;
        n.tw_end = tw_end;
        n.distID = nb_items;
        n.serv_time = serv_time;
        n.pick_node_id = pick_node_id - 1;
        n.del_node_id = del_node_id - 1;

        if (n.pick_node_id == -1)
            n.type = NODE_TYPE_PICKUP;
        else
            n.type = NODE_TYPE_DROP;

        pr.AddNode(n);

        vec_x.push_back(x);
        vec_y.push_back(y);

        sum_dmd += dmd;

    }

    fclose(f);

    //Requests creation
    k = 0;

    for (int i = 0; i < nb_items; i++) {
        Node *n = pr.GetNode(i);

        if (n->type == NODE_TYPE_PICKUP) {
            Request<Node> r;
            r.id = k;

            r.AddNode(n); //Add pick up node to request
            r.AddNode(pr.GetNode(n->del_node_id)); //Add correspondant delivery node to request
            pr.AddRequest(r);
            k++;
        }
    }

    for (int i = 0; i < nb_vehicles; i++) {
        Node dep1(dep); //Copy constructor
        dep1.id = nb_items + i * 2;
        Node dep2(dep);
        dep2.id = nb_items + (i * 2) + 1;
        dep2.type = NODE_TYPE_END_DEPOT;

        Driver d;
        d.capacity = cap;
        d.StartNodeID = dep1.id;
        d.EndNodeID = dep2.id;
        d.id = i;

        pr.AddNode(dep1);
        pr.AddNode(dep2);
        pr.AddDriver(d);

        //printf("Added driver %d with start depot %d and end depot %d\n", d.id, d.StartNodeID, d.EndNodeID);
    }


    int dim = (int) vec_x.size();
    double **d = new double *[dim];

    for (int i = 0; i < dim; i++) {
        d[i] = new double[dim];
        for (int j = 0; j < dim; j++) {
            d[i][j] = sqrt(
                    (vec_x[i] - vec_x[j]) * (vec_x[i] - vec_x[j]) + (vec_y[i] - vec_y[j]) * (vec_y[i] - vec_y[j]));
            //d[i][j] = (int)(d[i][j] + 0.5);
            //d[i][j] = ceil( d[i][j] - 0.0000001);
        }
    }

    pr.SetMaxtrices(d, d, dim);


}
