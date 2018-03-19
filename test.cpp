#include<iostream> 
#include "include/DCEL.h"
using namespace std;

int main(){
	vector< Point<int> > P;
	vector< pair<int, int> > E;
	int N, M;
	cin >> N >> M;
	for(int i = 0;i<N;i++){
		int x; int y;
		cin >> x >> y;
		Point<int> X(x, y);
		P.push_back(X);	
	}
	for(int i = 0;i<M;i++){
		int x, y;
		cin >> x >> y;
		E.push_back(make_pair(x,y));
	}
	DCEL<int> DD(P, E);
	DD.print();
	Point<int> U(0,2), V(-1,-1);
	shared_ptr< Vertex<int> > u, v;
	for(int i = 0;i<DD.VT.size();i++){
		if(DD.VT[i]->Coord == U)
			u = DD.VT[i];
		if(DD.VT[i]->Coord == V)
			v = DD.VT[i];
	}
	cout << u->Coord.toString() << " | " << v->Coord.toString() << endl;	
	cout << "after adding Edge" << endl;
	DD.addEdge(u, v, u->IE->IF);
	DD.print();
	V.x = 1; V.y = -1;
	cout << u->Coord.toString() << " | " << v->Coord.toString() << endl;	
	for(int i = 0;i<DD.VT.size();i++){
		if(DD.VT[i]->Coord == U)
			u = DD.VT[i];
		if(DD.VT[i]->Coord == V)
			v = DD.VT[i];
	}
	cout << u->Coord.toString() << " | " << v->Coord.toString() << endl;	
	cout << "after adding Edge" << endl;
	DD.addEdge(u, v, u->IE->IF);
	DD.print();

	return 0;
}
