#ifndef MEDIAN_H
#define MEDIAN_H

#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;
template<typename T>
T median(vector<T>);
template<typename T>
T medianOf5(int l, int r, vector<T> &arr){
    sort(arr.begin() + l, arr.begin() + r + 1);
    return arr[l + (r-l)/2];
}
template<typename T>
void swap(int l, int r, vector<T> &arr){
    T temp = arr[l];
    arr[l] = arr[r];
    arr[r] = temp;
}
template<typename T>
int partition(int l, int r, T k, vector<T> &arr){
    int i = l;
    for(int j = l;j<=r;j++){
        if(arr[j] <= k){
            swap(i,j,arr);
            i++;
        }
    }
    return i-1;

}
template<typename T>
T medianUtil(int l, int r, int k, vector<T> &arr){
    if(l <= k && k <= r){
    int i,j;
    int n = r-l + 1;
    vector<T> med;
    i = l;
   for(i = l;i+4<=r;i+=5){
       med.push_back(medianOf5(i,i+4,arr));
    }
   if(i<=r){
       med.push_back(medianOf5(i,i+(n%5)-1,arr));
    }
   T medofmed;
   if(med.size() == 1)
       medofmed = med[0];
    else
        medofmed = median(med);
    int pos = partition(l,r,medofmed,arr);
    if(pos == k)
        return medofmed;
    if(pos < k)
        return medianUtil(pos+1, r,k, arr);
    return medianUtil(l,pos-1,k,arr);


    }
    return (T)INT_MAX;
}
template<typename T>
T median(vector<T> arr){
    if(arr.size() == 0)
        return 0;
    if(arr.size() == 1)
        return arr[0];
    int reqPos = arr.size()/2;
    T x = medianUtil(0, arr.size() - 1, reqPos, arr);
    return x;
}
#endif
