/*
 * Copyright Jean-Francois Cote 2015
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 *
 */
#ifndef EXCHANGE
#define EXCHANGE

#include "Solution.h"
#include "IAlgorithm.h"
#include "InsRmvMethod.h"
#include "../../lib/mathfunc.h"



template <class NodeT, class DriverT, class MoveT>
class Exchange : public IAlgorithm<NodeT,DriverT>
{

public:
   Exchange(InsRmvMethod<NodeT,DriverT,MoveT> * insrmv):_insrmv(insrmv){}


   void Optimize(Sol<NodeT,DriverT> & s)
   {
      //printf("Exchange SolCost:%.3lf\n", s.GetCost());

      int move_count = 0;
      bool found = true;
      while(found)
      {
         found = false;

         requests.clear();
         for(int i = 0 ; i < s.GetRequestCount() ; i++)
            if(s.GetAssignedTo(s.GetRequest(i)) != NULL)
               requests.push_back(s.GetRequest(i));

         for(size_t i=0;i<requests.size();i++)
         {
            int index1 = mat_func_get_rand_int(0, (int)requests.size());
            int index2 = mat_func_get_rand_int_diff(0, (int)requests.size(),index1);
            Request<NodeT> * r = requests[index1];
            requests[index1] = requests[index2];
            requests[index2] = r;
         }


         for(size_t i = 0;i<requests.size();i++)
         {

            Request<NodeT> * r1 = requests[i];
            DriverT * d1 = s.GetAssignedTo(r1);
            Move<NodeT,DriverT,MoveT> r1_rem;
            double c1 = s.GetCost(d1);
            _insrmv->RemoveCost(s,r1,r1_rem);
            s.Remove(r1);
            s.Update(d1);
            double c2 = s.GetCost(d1);
            //printf("req:%d rmv:%.2lf old:%.2lf new:%.2lf\n",r1->id, r1_rem.DeltaCost, c1,c2);


            bool found_improv = false;
            for(size_t j = i+1;j<requests.size();j++)
            {
               Request<NodeT> * r2 = requests[j];
               DriverT * d2 = s.GetAssignedTo(r2);
               if(d1 == d2) continue;

               Move<NodeT,DriverT,MoveT> r2_rem;
               _insrmv->RemoveCost(s,r2,r2_rem);

               s.Remove(r2);
               s.Update(d2);

               Move<NodeT,DriverT,MoveT> r1_ins;
               Move<NodeT,DriverT,MoveT> r2_ins;
               _insrmv->InsertCost(s,r1,d2,r1_ins);
               _insrmv->InsertCost(s,r2,d1,r2_ins);

               //printf("r1:%d r2:%d ", r1->id,r2->id);
               //printf("r1ins:%lf r2ins:%lf ", r1_ins.DeltaCost, r2_ins.DeltaCost);
               //printf("r1rmv:%lf r2rmv:%lf ", r1_rem.DeltaCost, r2_rem.DeltaCost);
               //printf("sum:%lf\n", r2_rem.DeltaCost+r1_rem.DeltaCost+r1_ins.DeltaCost+r2_ins.DeltaCost);

               if(r2_rem.DeltaCost+r1_rem.DeltaCost+r1_ins.DeltaCost+r2_ins.DeltaCost < -0.0001)
               {
                  _insrmv->ApplyInsertMove(s,r2_ins);
                  _insrmv->ApplyInsertMove(s,r1_ins);
                  s.Update(d2);
                  s.Update(d1);

                  //if(move_count % 1000 == 0)
                  //printf("cnt:%d r1:%d d1:%d r2:%d d2:%d cost:%lf\n",move_count,r1->id,d1->id,r2->id,d2->id,s.GetCost());

                  move_count++;
                  found = true;
                  found_improv = true;
                  break;
               }
               else
               {
                  r2_rem.to = r2_rem.from;
                  r2_rem.from = NULL;
                  _insrmv->ApplyInsertMove(s,r2_rem);
                  s.Update(d2);
               }
            }//end for each neighbor

            if(!found_improv)
            {
               r1_rem.to = r1_rem.from;
               r1_rem.from = NULL;
               _insrmv->ApplyInsertMove(s,r1_rem);
               s.Update(d1);
            }

         }//end for each node
      }//end while
      //printf("Final Cost:%.3lf\n", s.GetCost());
      found = true;
   }


	private:
		InsRmvMethod<NodeT,DriverT,MoveT> * _insrmv;
		std::vector< Request<NodeT>* > requests;
};


#endif
