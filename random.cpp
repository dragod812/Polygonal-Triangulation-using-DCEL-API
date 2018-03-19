#include<iostream>
#include<memory>
using namespace std;
#define FPRINT(X) cout.width(10); cout << left << X 
void func(shared_ptr<int> &A)
{
	shared_ptr<int> P(new int(7));
	A = P;
	cout << P.use_count() << endl;
	cout << *P << endl;
}
int main()
{
	shared_ptr<int> A;
	func(A);
	cout << A.use_count() << endl;
	int x = 5, y = 2000;
	string res = "(" + to_string(x) + "," + to_string(y) + ")";
	FPRINT(res);
	return 0;
}
