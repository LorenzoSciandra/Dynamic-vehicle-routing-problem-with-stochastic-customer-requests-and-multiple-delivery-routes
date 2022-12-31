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

#ifndef CREATE_DRIVER_H
#define CREATE_DRIVER_H


template<class NodeT, class DriverT>
class Sol;

template<class NodeT, class DriverT>
class CreateDriver {
public:
    CreateDriver() {}

    virtual ~CreateDriver() {}

    virtual void Create(Sol<NodeT, DriverT> *p) = 0;
};

#endif
