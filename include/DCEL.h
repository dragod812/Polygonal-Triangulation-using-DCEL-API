#ifndef DCEL_H
#define DCEL_H
#include<iostream>
#include<vector>
#include<utility> 
#include<queue>
#include<set>
#include<memory>
#include<algorithm>
#include "Point.h"
using namespace std;
#define FPRINT(X) cout.width(20); cout << left << X 
#define NEXTLINE cout << endl
template<class T> class Edge;

enum VertexType { REGULAR, START, END, SPLIT, MERGE };

template<class T>
class Vertex{
	public:
    Point<T> Coord;
    VertexType Type;
    shared_ptr< Edge<T> > IE;
	Vertex(){
	}
	Vertex(Point<T> P){
		Coord = P;
		IE = NULL;
        Type = REGULAR;
	}
    Vertex(T x, T y, shared_ptr< Edge<T> > ie){
		Point<T> P(x,y);

        Coord = P;
		IE = ie;
    }
    string getType(){
        switch(Type){
            case REGULAR:
                 return "REGULAR";
            case START:
                return "START";
            case END:
                return "END";
            case SPLIT:
                return "SPLIT";
            case MERGE:
                return "MERGE";
        }
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
	shared_ptr< Vertex<T> > Helper;
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
	void setHelper(shared_ptr< Vertex<T> > v){
		Helper = v;
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
class Tau{
public:
	T X;
	shared_ptr< Edge<T> > E;
	Tau(){}
	Tau(T x){
		X = x;
	}
	Tau(T x, shared_ptr< Edge<T> > e){
		X = x;
		E = e;
	}
	bool operator < (const Tau &r) const {
		return X < r.X;
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
	struct VTCompare{
		bool operator() ( shared_ptr< Vertex<T> > l, shared_ptr< Vertex<T> > r ){
			if(l->Coord.y == r->Coord.y){
				return l->Coord.x < r->Coord.x; 
			}
			return l->Coord.y > r->Coord.y;
		}
	};
	
	//Initialise DCEL
	//Edge Index in 0 indexed
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
		
		//Fix IE of each vertex
		fixIE();
		
        //Set up Type of Each Vertex
        for(int i = 0;i<VT.size();i++){

            shared_ptr< Vertex<T> > X = VT[i];
            cout << "Cur: " <<  X->Coord.toString() ;
            shared_ptr< Vertex<T> > N = X->IE->Next->Origin;
            cout << "Next: " <<  N->Coord.toString() ;
            shared_ptr< Vertex<T> > P = X->IE->Prev->Origin;
            cout << "Prev: " <<  P->Coord.toString() << endl;

            //START/SPLIT
            if(N->Coord.y < X->Coord.y && P->Coord.y < X->Coord.y){
                if(N->Coord.x < P->Coord.x)
                    X->Type = START;
                else
                    X->Type = SPLIT;
            }
            //END/MERGE
            else if(N->Coord.y > X->Coord.y && P->Coord.y > X->Coord.y){
                if(N->Coord.x > P->Coord.x)
                    X->Type = END;
                else
                    X->Type = MERGE;
            }
            //REGULAR
            else{
                X->Type = REGULAR;
            }
        }
	}
    //Fixing the incident edges of each vertex so that its always an interior Edge.
    void fixIE(){
        for(int i = 0;i<VT.size();i++){
            Point<T> s = VT[i]->Coord;
            Point<T> e = (VT[i]->IE->Twin->Origin->Coord);
            Point<T> X = e - s;
            Point<T> P;
            if(VT[0]->Coord != s && VT[0]->Coord != e){
                P = VT[0]->Coord;
            }
            else if(VT[1]->Coord != s && VT[1]->Coord != e){
                P = VT[1]->Coord;
            }
            else
                P = VT[2]->Coord;
           	P = s - P;
			Vertex<T> EP(X), SP(P);
			if( SP*EP < 0 )
				VT[i]->IE = VT[i]->IE->Twin->Next;	
        }
    }
	// Add an Edge by splitting the current face into two
	void addEdge(shared_ptr< Vertex<T> > u, shared_ptr< Vertex<T> > v, shared_ptr< Face<T> > f){
		shared_ptr< Edge<T> > X = f->OuterComp;
		shared_ptr< Edge<T> > UE(new Edge<T>(u));
		shared_ptr< Edge<T> > VE(new Edge<T>(v));
		UE->Twin = VE;
		VE->Twin = UE;
		while(X->Origin != u)
			X = X->Next;
		UE->Prev = X->Prev;
		X->Prev->Next = UE;
		VE->Next = X;
		X->Prev = VE;
		shared_ptr< Face<T> > f1(new Face<T>(X));
		VE->IF = f1;
		while(X->Origin != v)
		{
			X->IF = f1;
			X = X->Next;
		}
		VE->Prev = X->Prev;
		X->Prev->Next = VE;
		UE->Next = X;
		X->Prev = UE;
		shared_ptr< Face<T> > f2(new Face<T>(X));
		UE->IF = f2;
		while(X->Origin != u)
		{
			X->IF = f2;
			X = X->Next;
		}
		for(int i = 0;i<FT.size();i++){
			if(FT[i].get() == f.get()){
				FT.erase(FT.begin()+i);
			}
		}
		FT.push_back(f1);
		FT.push_back(f2);
		ET.push_back(UE);
		ET.push_back(VE);
		// from outerComp to U or V
		/*
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
		*/
	}
	void HandleStartVertex(shared_ptr< Vertex<T> > V, set< Tau<T> > &TauSet){
		Tau<T> T1(V->Coord.x, V->IE);
		V->IE->Helper = V;
		TauSet.insert(T1);
	}
	void HandleEndVertex(shared_ptr< Vertex<T> > V, set< Tau<T> > &TauSet){
		shared_ptr< Edge<T> > Eminus1 = V->IE->Prev;
		if( Eminus1->Helper->Type == MERGE ){
			addEdge(V, Eminus1->Helper, Eminus1->IF);
		}
		Tau<T> T1(Eminus1->Origin->Coord.x, Eminus1);

		auto it = TauSet.find(T1);
		TauSet.erase(it);
	}
	void HandleSplitVertex(shared_ptr< Vertex<T> > V, set< Tau<T> > &TauSet){
		Tau<T> T1(V->Coord.x);
		auto itr = TauSet.lower_bound(T1);
		itr--;
		addEdge(V, itr->E->Helper, itr->E->IF);
		itr->E->Helper = V ;
		Tau<T> T2(V->Coord.x, V->IE);
		TauSet.insert(T2);
		V->IE->Helper = V;
	}	
	void HandleMergeVertex(shared_ptr< Vertex<T> > V, set< Tau<T> > &TauSet){
		//if Eminus1->helper = merge => addEDge
		shared_ptr< Edge<T> > Eminus1 = V->IE->Prev;
		if( Eminus1->Helper->Type == MERGE ){
			addEdge(V, Eminus1->Helper, Eminus1->IF);
		}
		//delete Eminus1
		Tau<T> T1(Eminus1->Origin->Coord.x, Eminus1);
		auto it = TauSet.find(T1);
		TauSet.erase(it);
		//search for ej				
		Tau<T> T2(V->Coord.x);
		auto itr = TauSet.lower_bound(T2);
		itr--;
		shared_ptr< Edge<T> > E = itr->E;
		if( E->Helper->Type == MERGE ){
			addEdge(V, E->Helper, E->IF);
		}
		E->Helper = V;
	}
	void HandleRegularVertex(shared_ptr< Vertex<T> > V, set< Tau<T> > &TauSet){
		shared_ptr< Vertex<T> > EndV = V->IE->Twin->Origin;
		if(EndV->Coord.y < V->Coord.y){
			shared_ptr< Edge<T> > Eminus1 = V->IE->Prev;
			if( Eminus1->Helper->Type == MERGE ){
				addEdge(V, Eminus1->Helper, Eminus1->IF);
			}
			Tau<T> T1(Eminus1->Origin->Coord.x, Eminus1);

			auto it = TauSet.find(T1);
			TauSet.erase(it);
			Tau<T> T2(V->Coord.x, V->IE);
			TauSet.insert(T2);
			V->IE->Helper = V;
		}
		else{
			Tau<T> T1(V->Coord.x);
			auto itr = TauSet.lower_bound(T1);
			itr--;
			shared_ptr< Edge<T> > E = itr->E;
			if( E->Helper->Type == MERGE ){
				addEdge(V, E->Helper, E->IF);
			}
			E->Helper = V;		
		}
	}	
    void MakeMonotone(){
		sort(VT.begin(), VT.end(), VTCompare());
		set< Tau<T> > TauSet;
		cout << "MAKE MONOTONE" << endl;
		for(int i = 0;i<VT.size();i++){
			cout << VT[i]->Coord.toString() << endl;
			switch(VT[i]->Type){
				case REGULAR:
					HandleRegularVertex(VT[i], TauSet);
					break;
				case START:
					HandleStartVertex(VT[i], TauSet);
					break;
				case END:
					HandleEndVertex(VT[i], TauSet);
					break;
				case SPLIT:
					HandleSplitVertex(VT[i], TauSet);
					break;
				case MERGE:
					HandleMergeVertex(VT[i], TauSet);
					break;
			}
			print();
		}
	}
	void print(){
		cout << "Vertex Table: " << endl;
		FPRINT("VERTEX"); FPRINT("COORD"); FPRINT("IE"); FPRINT("TYPE"); NEXTLINE;
		for(int i = 0;i<VT.size();i++){
			
			FPRINT(VT[i].get()); FPRINT(VT[i]->Coord.toString()); FPRINT(VT[i]->IE.get()); FPRINT(VT[i]->getType());NEXTLINE;
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
