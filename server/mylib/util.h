#include <string>
#include <stdlib.h>
#include <time.h>
using namespace std;
string randCode(){
    string ret;
    ret.resize(6);
    for(int i = 0;i<6;i++)
        ret[i] = 'a' + rand() % 26;
    return ret;
}