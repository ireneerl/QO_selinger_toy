#include <iostream>
#include <string>


void getCombination(int data[], int l, int r, int size){
    if(l==r){
        for(int i=0; i < size; i++){
            std::cout << data[i];
        }
        std::cout <<std::endl;
    }else{
        for (int i = l; i <=r; i++){
            std::swap(data[l], data[i]);
            getCombination(data, l+1, r, size);
            std::swap(data[l], data[i]);
        }
    }
}

int main(){
    int n = 5;
    int data[n];
    for (int i = 0 ; i < n ; i++){
        data[i] = i;
    }
    getCombination(data, 0, n-1, n);
}
