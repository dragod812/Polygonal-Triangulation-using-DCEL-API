#include<bits/stdc++.h>
using namespace std;
class Tau{
	public:
	int X;
	Tau(){}
	Tau(int x){
		X = x;
	}
	bool operator < (const Tau &r) const {
		return X < r.X;
	}
};
void func(vector<int> arr){
	arr.push_back(4);
}
int main(){
	set< Tau > st;
	Tau a(1), b(2), c(6);
	st.insert(a);
	st.insert(b);
	st.insert(c);
	Tau d(7);
	auto x = st.find(d);
	x--;
	cout << x->X << endl;
		cout << "x";
for( auto itr = st.begin(); itr != st.end();itr++){
		cout << itr->X << " ";
	}
	return 0;
}
