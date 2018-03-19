#ifndef LINE_H
#define LINE_H

#include "Point.h"

template<class T>
class Line{
public:
   Point<T> p1, p2;
   double M, C;
   Line(){
       M = 0;
       C = 0;
    }
   Line(Point<T> p1, Point<T> p2){
       this->p1 = p1;
       this->p2 = p2;
       calculateParams();
    }
   Line(Point<T> p1, double slope){
       M = slope;
       C = p1.y - M*p1.x;
    }
   void calculateParams(){
       M = (double)(p1.y - p2.y)/(p1.x - p2.x);
       C = (double)p1.y - M*p1.x;
    }
   double getSlope(){
       return M;
    }
   double getIntercept(){
       return C;
    }
};
#endif
