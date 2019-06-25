
#include <stdio.h>

int test_function(int* a, int* b, int n) {

    int op1 = *a;
    int op2 = *b;
    *b = n+2;
   //DFGLoop:loop
   return   (op1 + op2)*(op1-op2)*(n+5);
    //return sum;
}

