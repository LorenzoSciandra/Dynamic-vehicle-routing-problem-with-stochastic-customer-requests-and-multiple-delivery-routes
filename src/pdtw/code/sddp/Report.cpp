#include "Report.h"
#include "Parameters.h"

void Report::Build(Sol<Node, Driver> &s) {
    //s.Show();
    GetDriverCountAtDepot(s);
    GetWaitingAtDelivery(s);
    //Show();
}

void Report::Show() {
    printf("coordinates {");
    for (int t = 0; t <= Parameters::GetTimeHorizon(); t++)
        printf("(%d,%d) ", t, drivers_at_depot[t]);
    printf("};\n");
    /*for(int t=0;t<Parameters::GetTimeHorizon();t++)
    {
        printf("Time:%d Drivers at depot:%d ", t,drivers_at_depot[t] );
        printf("WaitingAtDelivery:%d \n", waiting_at_delivery[t]);
    }*/
}

double Report::GetIdleRate() {
    double sum = 0;
    for (int t = 0; t <= Parameters::GetTimeHorizon(); t++)
        sum += drivers_at_depot[t];
    sum /= Parameters::GetTimeHorizon();
    sum /= Parameters::GetDriverCount();
    return sum;
}

void Report::GetDriverCountAtDepot(Sol<Node, Driver> &s) {
    drivers_at_depot.clear();
    drivers_at_depot.resize(Parameters::GetTimeHorizon() + 1, 0);

    for (int t = 0; t <= Parameters::GetTimeHorizon(); t++)
        for (int i = 0; i < s.GetDriverCount(); i++) {
            Driver *d = s.GetDriver(i);
            Node *prev = s.GetNode(d->StartNodeID);
            while (prev != NULL) {
                if (prev->type == NODE_TYPE_START_DEPOT) {
                    Node *next = s.GetNext(prev);
                    if (t <= next->arrival_time) {
                        drivers_at_depot[t]++;
                        break;
                    }
                } else if (prev->type == NODE_TYPE_END_DEPOT && prev->arrival_time <= t) {
                    drivers_at_depot[t]++;
                    break;
                } else if (prev->type == NODE_TYPE_PICKUP && prev->arrival_time <= t && prev->departure_time > t) {
                    drivers_at_depot[t]++;
                    break;
                }

                prev = s.GetNext(prev);
            }
        }
}

void Report::GetWaitingAtDelivery(Sol<Node, Driver> &s) {
    waiting_at_delivery.clear();
    waiting_at_delivery.resize(Parameters::GetTimeHorizon() + 1, 0);


    for (int t = 0; t <= Parameters::GetTimeHorizon(); t++)
        for (int i = 0; i < s.GetDriverCount(); i++) {
            Driver *d = s.GetDriver(i);
            Node *prev = s.GetNode(d->StartNodeID);
            while (prev != NULL) {
                if (prev->type == NODE_TYPE_DROP && prev->arrival_time <= t && prev->tw_start > t) {
                    waiting_at_delivery[t]++;
                    break;
                }

                prev = s.GetNext(prev);
            }
        }

}
