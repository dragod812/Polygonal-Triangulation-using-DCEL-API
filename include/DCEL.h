#ifndef DCEL_H
#define DCEL_H
#include<iostream>
#include<vector>
#include<utility> 
#include<queue>
#include<memory>
#include<algorithm>
#include "Point.h"
using namespace std;
#define FPRINT(X) cout.width(20); cout << left << X 
#define NEXTLINE cout << endl
template<class T> class Edge;

template<class T>
class Vertex{
	public:
    Point<T> Coord;
    shared_ptr< Edge<T> > IE;
	Vertex(){
	}
	Vertex(Point<T> P){
		Coord = P;
		IE = NULL;
	}
    Vertex(T x, T y, shared_ptr< Edge<T> > ie){
		Point<T> P(x,y);

        Coord = P;
		IE = ie;
    }
	Vertex operator - (const Vertex &rhs){
		Point<T> P(Coord.x-rhs.Coord.x,Coord.y-rhs.Coord.y); 
		return Vertex(P);
    }
	T operator * (const Vertex &rhs){
		return (Coord.x*rhs.Coord.y - Coord.y*rhs.Coord.x);
	}

};

template<class T>
class Face{
	public:
    shared_ptr< Edge<T> > OuterComp;
	Face(){
	}
    Face(shared_ptr< Edge<T> > OC){
        OuterComp = OC;
    }
};

template<class T>
class Edge{
	public:
    shared_ptr< Vertex<T> > Origin;
	// for sorting in anticlockwise order to set up ET: Next, Prev
	// Origin Shifted End
	Vertex<T> OSE;
	double Angle;	
    shared_ptr< Edge<T> > Twin, Next, Prev;
    shared_ptr< Face<T> > IF;
	Edge(){
	}
    Edge(shared_ptr< Vertex<T> > O){
        Origin = O;
		IF = NULL;
    }
	void setOSE(){
		OSE = *Twin->Origin - *Origin;
	}
	void setOriginIE(){
		Origin->IE = this;
	}
	bool operator <( const shared_ptr< Edge<T> > &r ){
		if(Origin->Coord.x == r->Origin->Coord.x){
			if(Origin->Coord.y == r->Origin->Coord.y){
				return ((OSE)*(r->OSE)) > 0;	
			}
			return Origin->Coord.y < r->Origin->Coord.y; 
		}
		return Origin->Coord.x < r->Origin->Coord.x;
	}


};

template<class T>
class DCEL{
	public:
	vector< shared_ptr< Vertex<T> > > VT; // VertexTable
	vector< shared_ptr< Edge<T> > > ET; // EdgeTable
	vector<	shared_ptr< Face<T> > > FT; // FaceTable
	struct ETCompare{
		bool operator() ( shared_ptr< Edge<T> > l, shared_ptr< Edge<T> > r ){
			if(l->Origin->Coord.x == r->Origin->Coord.x){
				if(l->Origin->Coord.y == r->Origin->Coord.y){
					return ((l->OSE)*(r->OSE)) > 0;	
				}
				return l->Origin->Coord.y < r->Origin->Coord.y; 
			}
			return l->Origin->Coord.x < r->Origin->Coord.x;
		}
	};
		
	//Initialise DCEL
	DCEL(vector< Point<T> > Points, vector< pair< int, int > > EdgeIndx ){
		// Set up VT: Push Points into VT 
		for(Point<T> P : Points){
			shared_ptr< Vertex<T> > V(new Vertex<T>(P));
			VT.push_back(V);
		}
		// Set up ET: give values to Twin Edges
		// Set up VT: Initialise IE
		for(pair<int, int> ei : EdgeIndx){
			shared_ptr< Edge<T> > E(new Edge<T>(VT[ei.first]));
			shared_ptr< Edge<T> > ETwin(new Edge<T>(VT[ei.second]));
			E->Twin = ETwin;
			ETwin->Twin = E;
			ET.push_back(E);
			ET.push_back(ETwin);
			VT[ei.first]->IE = E;
			VT[ei.second]->IE = ETwin;
		}
		// Set Up ET: Calculate OSE for sorting
		for(shared_ptr< Edge<T> > E : ET){
			E->setOSE();
		}	
		sort(ET.begin(), ET.end(), ETCompare());
		int indx = -1;
		// Set Up ET: give Values to Next and Prev Edges
		for(int i = 0;i<ET.size()-1;i++){
			if(ET[i]->Origin == ET[i+1]->Origin){
				if(indx == -1){
					indx = i;
					ET[i]->Origin->IE = ET[i];
				}
				ET[i]->Prev = ET[i+1]->Twin;
				ET[i+1]->Twin->Next = ET[i];
			}
			else{
				ET[i]->Prev = ET[indx]->Twin;
				ET[indx]->Twin->Next = ET[i];
				indx = -1;
			}
		}
		ET[ET.size() - 1]->Prev = ET[indx]->Twin;
		ET[indx]->Twin->Next = ET[ET.size() - 1];
		//Set Up FT: also find out Incident Faces for each Edge
		queue< shared_ptr< Edge<T> > > Q;
		Q.push( VT[0]->IE );
		while(!Q.empty()){
			shared_ptr< Edge<T> > X = Q.front();
			Q.pop();
			if(X->IF == NULL){
				shared_ptr< Face<T> > F(new Face<T>(X) );
				FT.push_back(F);
				do{
					X->IF = F;
					Q.push(X->Twin);
					X = X->Next;
				}while(X->IF == NULL);
			}
		}
	}
	// Add an Edge by splitting the current face into two
	void addEdge(shared_ptr< Vertex<T> > u, shared_ptr< Vertex<T> > v, shared_ptr< Face<T> > f){
		shared_ptr< Edge<T> > X = f->OuterComp;
		shared_ptr< Edge<T> > UE(new Edge<T>(u));
		shared_ptr< Edge<T> > VE(new Edge<T>(v));
		UE->Twin = VE;
		VE->Twin = UE;
		// from outerComp to U or V
		while(X->Origin != u && X->Origin != v){
			X = X->Next;
		}
		shared_ptr< Face<T> > f2(new Face<T>(X));
		if(X->Origin == u){
			UE->Prev = X->Prev;
			VE->Next = X;
			VE->IF = f2;
		}
		else{
			VE->Prev = X->Prev;
			UE->Next = X;
			UE->IF = f2;
		}
		//From U or V to V or U
		do{
			X->IF = f2;
			X = X->Next;
		}while(X->Origin != u && X->Origin != v);
		
		shared_ptr< Face<T> > f1(new Face<T>(X));
		if(X->Origin == u){
			UE->Prev = X->Prev;
			VE->Next = X;
			VE->IF = f1;
		}
		else{
			VE->Prev = X->Prev;
			UE->Next = X;
			UE->IF = f1;
		}

		// From V or U to U or V
		do{
			X->IF = f1;
			X = X->Next;
		}while(X->Origin != u && X->Origin != v);
		//erase previous face
		for(int i = 0;i<FT.size();i++){
			if( FT[i] == f){
				FT.erase(FT.begin() + i);
				break;
			}
		}
		//Add new Faces
		FT.push_back(f1);
		FT.push_back(f2);
		//Add New Edges
		ET.push_back(UE);
		ET.push_back(VE);
	}
	void print(){
		cout << "Vertex Table: " << endl;
		FPRINT("VERTEX"); FPRINT("COORD"); FPRINT("IE"); NEXTLINE;
		for(int i = 0;i<VT.size();i++){
			
			FPRINT(VT[i].get()); FPRINT(VT[i]->Coord.toString()); FPRINT(VT[i]->IE.get()); NEXTLINE;
		}
		cout << "Edge Table: " << endl;
		FPRINT("EDGE"); FPRINT("ORIGIN"); FPRINT("TARGET"); FPRINT("NEXT"); FPRINT("PREV"); FPRINT("IF"); NEXTLINE;
		for(int i = 0;i<ET.size();i++){
			FPRINT(ET[i].get()); FPRINT(ET[i]->Origin->Coord.toString()); FPRINT(ET[i]->Twin->Origin->Coord.toString()); FPRINT(ET[i]->Next.get()); FPRINT(ET[i]->Prev.get()); FPRINT(ET[i]->IF.get()); NEXTLINE;
		}

		cout << "Face Table: " << endl;
		FPRINT("FACE"); FPRINT("OUTERCOMP"); NEXTLINE;
		for(int i = 0;i<FT.size();i++){
			FPRINT(FT[i].get()); FPRINT(FT[i]->OuterComp.get()); NEXTLINE;
		}
	
	}
};
#endif
