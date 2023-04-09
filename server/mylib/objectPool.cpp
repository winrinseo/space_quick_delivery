#ifndef OBJECT__POOL__CPP
#define OBJECT__POOL__CPP
#include "objectPool.h"

template<typename T>
objectPool<T>::objectPool(){

};

template<typename T>
objectPool<T>::objectPool(int max){
    for(int i = 0;i<=max;i++){
        o.push(new T);
    }
};

template<typename T>
objectPool<T>::~objectPool(){
    while(!o.empty()){
        delete o.top();
        o.pop();
    }
};

template<typename T>
T * objectPool<T>::getObject(){
    if(o.empty()) return nullptr;
    T * ret = o.top(); o.pop();
    return ret;
}

template<typename T>
void objectPool<T>::putObject(T * obj){
    o.push(obj);
}

#endif