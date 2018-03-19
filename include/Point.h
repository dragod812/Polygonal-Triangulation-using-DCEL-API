#ifndef POINT_H
#define POINT_H
#include<iostream>
using namespace std;
template<class T>
class Point{
public:
	T x, y;
	Point(){
		x = 0; y = 0;
	}
	Point(T x, T y){
		this->x = x;
		this->y = y;
	}
    bool operator == (const Point &rhs) const {
        return (x == rhs.x && y == rhs.y);
    }
    bool operator != (const Point &rhs) const {
        return (x != rhs.x || y != rhs.y);
    }
    void print(){
        cout << "(" << x << "," << y << ")"<<endl;
    }
};
#endif
