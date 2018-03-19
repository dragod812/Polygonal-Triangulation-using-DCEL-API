#ifndef DCEL_H
#define DCEL_H
#include<iostream>
using namespace std;

template<class T>
class Vertex{
    Point<T> Coord;
    shared_ptr<Edge<T>> IncidentEdge;
    Vertex(T x, T y, shared_ptr<Edge<T>> IE){
        Coord = 
}
