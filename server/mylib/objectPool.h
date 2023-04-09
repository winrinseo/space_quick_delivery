#ifndef OBJECT__POOL__H
#define OBJECT__POOL__H
#include <stack>

template <typename T>
class objectPool{
private:
    std::stack<T*> o;

public:
    objectPool();
    objectPool(int max);
    ~objectPool();

    T * getObject();
    void putObject(T * obj);
};

#endif