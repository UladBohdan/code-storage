#ifndef BITS_H
#define BITS_H

#include <vector>
#include <algorithm>

using namespace std;

class bits
{
    private:
        unsigned long int data[4];
    public:
        bits() { for (int i=0; i<4; i++) data[i]=0; }
        void push(int);
        bool find(int);
        void pop(int);
        int* out();
        ~bits() {}
};


#endif // BITS_H
