#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H
#include<vector>
#include "Point.h"
#include "Median.h"
#include<iostream>
#include<utility>
#include<algorithm>
#include "Line.h"
using namespace std;


namespace CONVEX_HULL_ALGO{
	enum ALGORITHM { GRAHAMS_SCAN, JARVIS_MARCH, KIRKPATRICK_SEIDEL };
    enum HULL { UPPER, LOWER };
}
template<class T>
class ConvexHull{
public:
	vector< Point<T> > convexHull;
	vector< Point<T> > points;
	ConvexHull(vector< Point<T> > points, CONVEX_HULL_ALGO::ALGORITHM A){
		this->points = points;
		switch(A){
			case CONVEX_HULL_ALGO::GRAHAMS_SCAN:
				GrahamsScan();
				break;

			case CONVEX_HULL_ALGO::JARVIS_MARCH:
				JarvisMarch();
				break;
			case CONVEX_HULL_ALGO::KIRKPATRICK_SEIDEL:
				KirkpatrickSeidel();
				break;
		}
	}
	~ConvexHull(){

	}
private:
 	 Point<T>  lowestPoint;
    Point<T> UL, UR, LL, LR;
	vector< Point<T> > sortedPoints;
	void LowestIndex(){
		int minIndx = 0;
		for(size_t i = 0;i<points.size();i++){
			if(points[i].y < points[minIndx].y){
				minIndx = i;
			}
			else if(points[i].y == points[minIndx].y){
				minIndx = (points[i].x < points[minIndx].x)?(i):(minIndx);
			}
		}
		// lowest point is swapped with the 0th index
		lowestPoint = points[minIndx];
		points[minIndx] = points[0];
		points[0] = lowestPoint;
	}
    double abs(double x){
        double ans = (x >= 0)?x:(-1*x);
        return ans;
    }
	//form a vector from the given index point to all the points and store it in sorted
	void vectorise(int indx){
		for(size_t i = 0;i<sortedPoints.size();i++){
			if(i != indx){
				sortedPoints[i] = makeVector(sortedPoints[indx], sortedPoints[i]);
			}
		}
	}
	 Point<T>  makeVector(Point<T> p1, Point<T> p2){
		 Point<T>  p3(p2.x - p1.x, p2.y - p1.y);
		return p3;
	}
	//get the points back from the vectors given the reference index
	void unvectorise(int indx){
		for(size_t i = 0;i<sortedPoints.size();i++){
			if(i != indx){
				sortedPoints[i].x = sortedPoints[i].x + sortedPoints[indx].x;
				sortedPoints[i].y = sortedPoints[i].y + sortedPoints[indx].y;
			}
		}
	}
	T crossProduct( Point<T>  l, Point<T> r){
		return (l.x*r.y - r.x*l.y);
	}
	T magnitude2( Point<T>  l){
		return l.x*l.x + l.y*l.y;
	}
	static bool comparePoints(const Point<T> &l, const Point<T> &r) {
		return ((l.x*r.y - r.x*l.y) > 0);
	}
    static bool basisX(const  Point<T>  &l, const Point<T> &r) {
        return (l.x < r.x);
    }
    static bool basisY(const  Point<T>  &l, const Point<T> &r) {
        return (l.y < r.y);
    }
	void removeDegeneracies(){
		for(size_t i = 1;i<sortedPoints.size()-1;i++){
			if(crossProduct(sortedPoints[i], sortedPoints[i+1]) == 0){
				if(magnitude2(sortedPoints[i]) < magnitude2(sortedPoints[i+1])){
					sortedPoints.erase(sortedPoints.begin() + i);
				}
				else{
					sortedPoints.erase(sortedPoints.begin() + i + 1);
				}
				i--;
			}
		}
	}
	void GrahamsScan(){
		LowestIndex();
		sortedPoints = points;
		vectorise(0);
		sort(sortedPoints.begin()+1, sortedPoints.end(), comparePoints);
		removeDegeneracies();
		unvectorise(0);
		convexHull.push_back(sortedPoints[0]);
		convexHull.push_back(sortedPoints[1]);
        convexHull.push_back(sortedPoints[2]);
		for(size_t i = 3;i<sortedPoints.size();i++){
			 Point<T>  p2 = convexHull.back();
			convexHull.pop_back();
			 Point<T>  p1 = convexHull.back();
			 Point<T>  p3 = sortedPoints[i];
			 Point<T>  v2 = makeVector(p1, p2);
			 Point<T>  v3 = makeVector(p2, p3);
			if(crossProduct(v2,v3) >= 0){

				convexHull.push_back(p2);
			    convexHull.push_back(p3);

			}
            else
                i--;
		}

	}
     Point<T>  leftmostPoint;
	//Point leftmostPoint; //Contains the leftmost point

	void leftmostIndx(){  // Function to find the leftmost point.
		int leftmostIndex=0;
		for(size_t i=0;i<points.size();i++){
			if(points[i].x<points[leftmostIndex].x){
				leftmostIndex=i;
			}
		}
		leftmostPoint=points[leftmostIndex];
	}

	void JarvisMarch(){
		leftmostIndx();
		 Point<T>  pointOnHull=leftmostPoint;
		 Point<T>  endpoint;
		while(1){
			convexHull.push_back(pointOnHull);
			endpoint=points[0];
			for(size_t j=1;j<points.size();j++){
				 Point<T>  candidateEdge=makeVector(pointOnHull,endpoint);
				 Point<T>  temp=makeVector(pointOnHull,points[j]);
				if((crossProduct(candidateEdge,temp)<0)||(pointOnHull==endpoint)){
					endpoint=points[j];
				}
				else if(crossProduct(candidateEdge,temp)==0){
					if(magnitude2(candidateEdge)>magnitude2(temp)){
						continue;
					}
					else{
						endpoint=points[j];
					}
				}
				if(pointOnHull==points[j]){
					continue;
				}
			}
			pointOnHull=endpoint;
			if(endpoint==convexHull[0]){
				break;
			}
		}
	}
    void swap(int i, int j, vector<Point<T> > &P){

        Point<T> tempo = P[i];
        P[i] = P[j];
        P[j] = tempo;
    }
    void swap(Point<T> &p1, Point<T> &p2){
        Point<T> temp = p1;
        p1 = p2;
        p2 = temp;
    }
    void findSupportingLine(Point<T> &pk, Point<T> &pm, vector< Point<T> > S, double alpha){
        //MAXC stores the maximum possible intercept
        double MAXC = S[0].y - alpha*S[0].x;
        //maxCL: leftmost pt with maximum intercept
        //maxCR: rightmost pt with maximum intercept
        Point<T> maxCL = S[0], maxCR = S[0];
        for(size_t i = 1;i<S.size();i++){
            double curC = S[i].y - alpha*S[i].x;
            if(curC > MAXC){
                MAXC = curC;
                maxCL = maxCR = S[i];
            }

        }
        for(size_t i = 0;i<S.size();i++){
            double curC = S[i].y - alpha*S[i].x;
           if(curC == MAXC){
               if(maxCL.x > S[i].x)
                   maxCL = S[i];
               if(maxCR.x < S[i].x)
                   maxCR = S[i];
            }
        }
        pk = maxCL; pm = maxCR;
    }
    void upperBridge(Point<T> &pl, Point<T> &pr, vector< Point<T> > S, T med){
       vector< Point<T> > candidate;
       //base case
       if(S.size() == 2){
           if(S[0].x > S[1].x){
               swap(0,1,S);
            }
            pl = S[0];
            pr = S[1];
            return;
        }
       vector< Line<T> > lines;
       //arbitrary pairing of points to make lines
       for(size_t i = 0;i<S.size()-1;i+=2){
           if(S[i].x > S[i+1].x){
               swap(i,i+1,S);
            }
           if(S[i].x==S[i+1].x){
               Point< T > x = (S[i].y > S[i+1].y)?S[i]:S[i+1];
               candidate.push_back(x);
            }
           lines.push_back( Line<T>(S[i],S[i+1]));
        }
       //condition when number of points is odd
       if(S.size()%2 == 1)
           candidate.push_back(S[S.size() - 1]);
        vector< double > slopes;
        //calculate slope of each line
        for(int i = 0;i<lines.size();i++)
            slopes.push_back(lines[i].getSlope());
        //get median of slopes
        double alpha = median<double>(slopes);
        vector< Line<T> > SMALL, EQUAL, LARGE;
        //distribute the lines with slope smaller alpha into SMALL
        //greter than alpha into LARGE
        //equal to alpha into EQUAL
        for(int i = 0;i<lines.size();i++){
            if(lines[i].getSlope() < alpha)
                SMALL.push_back(lines[i]);
            else if(lines[i].getSlope() == alpha)
                EQUAL.push_back(lines[i]);
            else
                LARGE.push_back(lines[i]);
        }
        //pk, pm stores the points for the supporting line
        Point<T> pk, pm;
        findSupportingLine(pk, pm, S, alpha);

        if(pk.x <= med && pm.x > med)
        {
            pl = pk;
            pr = pm;
            return;
        }
        if(pm.x <= med){
            for(int i =0;i<SMALL.size();i++){
                candidate.push_back(SMALL[i].p1);
                candidate.push_back(SMALL[i].p2);
            }
             for(int i =0;i<EQUAL.size();i++){
                 candidate.push_back(EQUAL[i].p2);
            }

            for(int i =0;i<LARGE.size();i++){
                candidate.push_back(LARGE[i].p2);
            }

        }
        if(pk.x > med){
            for(size_t i =0;i<SMALL.size();i++){
                candidate.push_back(SMALL[i].p1);
            }
             for(size_t i =0;i<EQUAL.size();i++){
                 candidate.push_back(EQUAL[i].p1);
            }

            for(size_t i =0;i<LARGE.size();i++){
                candidate.push_back(LARGE[i].p2);
                candidate.push_back(LARGE[i].p1);
            }

        }
        upperBridge(pl,pr,candidate,med);
    }
    vector< Point<T> > upperHull(Point<T> pmin, Point<T> pmax, vector< Point<T> > P){

        if(P.size() == 2){

            vector< Point<T> > result;
            if(P[0].x < P[1].x){
                result.push_back(P[0]);
                result.push_back(P[1]);
                return result;
            }
            result.push_back(P[1]);
            result.push_back(P[0]);
            return result;
        }

        if(pmin == pmax)
        {
            vector< Point<T> > result;
            result.push_back(pmin);
            return result;
        }
        vector< T > X;
        for(size_t i = 0;i<P.size();i++){
            X.push_back(P[i].x);
        }
        T med = median<T>(X);
        Point<T> pl, pr;
        upperBridge(pl, pr, P, med);
        //excluded for the recursion step
        vector< Point<T> > Pleft, Pright;
        Point<T> v1 = makeVector(pmin, pl), v2 = makeVector(pr, pmax);
        for(size_t i = 0;i<P.size();i++){
            if(P[i] != pl && P[i] != pr && P[i] != pmin && P[i] != pmax){
                Point<T> vl = makeVector(pl, P[i]), vr = makeVector(pmax,P[i]);
                if(crossProduct(v1,vl) > 0){
                    Pleft.push_back(P[i]);
                }
                if(crossProduct(v2,vr) > 0){
                    Pright.push_back(P[i]);
                }
            }
        }
        Pleft.push_back(pl);
        if(pl != pmin)
            Pleft.push_back(pmin);
        Pright.push_back(pr);
        if(pr != pmax)
            Pright.push_back(pmax);
        vector< Point<T> > LUH, RUH;
        LUH = upperHull(pmin,pl,Pleft);
        RUH = upperHull(pr,pmax,Pright);
        for(size_t i = 0;i<RUH.size();i++){
            LUH.push_back(RUH[i]);
        }
        return LUH;
    }
    void findLowerSupportingLine(Point<T> &pk, Point<T> &pm, vector< Point<T> > S, double alpha){
        //MAXC stores the maximum possible intercept
        double MINC = S[0].y - alpha*S[0].x;
        //maxCL: leftmost pt with maximum intercept
        //minCR: rightmost pt with maximum intercept
        Point<T> minCL = S[0], minCR = S[0];
        for(size_t i = 1;i<S.size();i++){
            double curC = S[i].y - alpha*S[i].x;
            if(curC < MINC){
                MINC = curC;
                minCL = minCR = S[i];
            }

        }
        for(size_t i = 0;i<S.size();i++){
            double curC = S[i].y - alpha*S[i].x;
           if(curC == MINC){
               if(minCL.x > S[i].x)
                   minCL= S[i];
               if(minCR.x < S[i].x)
                   minCR = S[i];
            }
        }
        pk = minCL; pm = minCR;
    }

    void lowerBridge(Point<T> &pl, Point<T> &pr, vector< Point<T> > S, T med){
       vector< Point<T> > candidate;
       //base case
       if(S.size() == 2){
           if(S[0].x > S[1].x){
               swap(0,1,S);
            }
            pl = S[0];
            pr = S[1];
            return;
        }
       vector< Line<T> > lines;
       //arbitrary pairing of points to make lines
       for(size_t i = 0;i<S.size()-1;i+=2){
           if(S[i].x > S[i+1].x){
               swap(i,i+1,S);
            }
           if(S[i].x==S[i+1].x){
               Point< T > x = (S[i].y < S[i+1].y)?S[i]:S[i+1];
               candidate.push_back(x);
            }
           lines.push_back( Line<T>(S[i],S[i+1]));
        }
       //condition when number of points is odd
       if(S.size()%2 == 1)
           candidate.push_back(S[S.size() - 1]);
        vector< double > slopes;
        //calculate slope of each line
        for(size_t i = 0;i<lines.size();i++)
            slopes.push_back(lines[i].getSlope());
        //get median of slopes
        double alpha = median<double>(slopes);
        vector< Line<T> > SMALL, EQUAL, LARGE;
        //distribute the lines with slope smaller alpha into SMALL
        //greter than alpha into LARGE
        //equal to alpha into EQUAL
        for(size_t i = 0;i<lines.size();i++){
            if(lines[i].getSlope() < alpha)
                SMALL.push_back(lines[i]);
            else if(lines[i].getSlope() == alpha)
                EQUAL.push_back(lines[i]);
            else
                LARGE.push_back(lines[i]);
        }
        //pk, pm stores the points for the supporting line
        Point<T> pk, pm;
        findLowerSupportingLine(pk, pm, S, alpha);

        if(pk.x <= med && pm.x > med)
        {
            pl = pk;
            pr = pm;
            return;
        }
        if(pm.x <= med){
            for(size_t i =0;i<SMALL.size();i++){
                candidate.push_back(SMALL[i].p2);
            }
             for(int i =0;i<EQUAL.size();i++){
                 candidate.push_back(EQUAL[i].p2);
            }

            for(int i =0;i<LARGE.size();i++){
                candidate.push_back(LARGE[i].p1);
                candidate.push_back(LARGE[i].p2);
            }

        }
        if(pk.x > med){
            for(int i =0;i<SMALL.size();i++){
                candidate.push_back(SMALL[i].p1);
                candidate.push_back(SMALL[i].p2);
            }
             for(int i =0;i<EQUAL.size();i++){
                 candidate.push_back(EQUAL[i].p1);
            }

            for(int i =0;i<LARGE.size();i++){
                candidate.push_back(LARGE[i].p1);
            }

        }
        lowerBridge(pl,pr,candidate,med);
    }

    vector< Point<T> > lowerHull(Point<T> pmin, Point<T> pmax, vector< Point<T> > P){


        if(pmin == pmax)
        {
            vector< Point<T> > result;
            result.push_back(pmin);
            return result;
        }
        vector< T > X;
        for(int i = 0;i<P.size();i++){
            X.push_back(P[i].x);
        }
        T med = median<T>(X);
        Point<T> pl, pr;
        lowerBridge(pl, pr, P, med);
        //excluded for the recursion step
        vector< Point<T> > Pleft, Pright;
        Point<T> v1 = makeVector(pmin, pl), v2 = makeVector(pr, pmax);
        for(int i = 0;i<P.size();i++){
            if(P[i] != pl && P[i] != pr && P[i] != pmin && P[i] != pmax){
                Point<T> vl = makeVector(pl, P[i]), vr = makeVector(pmax,P[i]);
                if(crossProduct(v1,vl) < 0){
                    Pleft.push_back(P[i]);
                }
                if(crossProduct(v2,vr) < 0){
                    Pright.push_back(P[i]);
                }
            }
        }
        Pleft.push_back(pl);
        if(pl != pmin)
            Pleft.push_back(pmin);
        Pright.push_back(pr);
        if(pr != pmax)
            Pright.push_back(pmax);
        vector< Point<T> > LUH, RUH;
        LUH = lowerHull(pmin,pl,Pleft);
        RUH = lowerHull(pr,pmax,Pright);
        for(int i = 0;i<RUH.size();i++){
            LUH.push_back(RUH[i]);
        }
        return LUH;
    }

	void KirkpatrickSeidel(){
        //calculate the Upper-Left-Extreme, Upper-Right-Extreme,
        //Lower-Left-Extreme, Lower-right-Extreme
        UL = points[0]; UR = points[0]; LL = points[0]; LR = points[0];
        for(size_t i = 0;i<points.size();i++){
            if(UL.x > points[i].x)
                UL = points[i];
            else if(UL.x == points[i].x)
                if(UL.y < points[i].y)
                    UL = points[i];

            if(LL.x > points[i].x)
                LL = points[i];
            else if(LL.x == points[i].x)
                if(LL.y > points[i].y)
                    LL = points[i];

            if(UR.x < points[i].x)
                UR = points[i];
            else if(UR.x == points[i].x)
                if(UR.y < points[i].y)
                    UR = points[i];

             if(LR.x < points[i].x)
                LR = points[i];
            else if(LR.x == points[i].x)
                if(LR.y > points[i].y)
                    LR = points[i];

        }
        vector< Point<T> > candidate;
        for(size_t i = 0;i<points.size();i++){
            if(UL.x != points[i].x && UR.x != points[i].x){
               candidate.push_back(points[i]);
            }
        }
        candidate.push_back(UL);
        candidate.push_back(UR);
        vector< Point<T> > upper = upperHull(UL, UR, candidate);
        candidate.pop_back();
        candidate.pop_back();
        candidate.push_back(LL);
        candidate.push_back(LR);
        vector< Point<T> > lower = lowerHull(LL, LR, candidate);
        int i = lower.size() - 1, end = 0;
        if(upper.back() == lower[i]) i--;
        if(upper.front() == lower[0]) end++;
        for(;i>=end;i--){
                upper.push_back(lower[i]);
        }
        convexHull = upper;
	}

};
#endif
