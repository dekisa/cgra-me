#include <stdio.h>

int __attribute__ ((noinline)) do_procA(int a, int b){
    //DFGLoop:processingA
    return a+b-5;
}



int __attribute__ ((noinline)) do_procB(int a, int b){
    //DFGLoop:processingB
    return a-b;
}

int main(){
    int a = 5;
    int b = 3;

    int c = do_procA(a,b);

    do_procB(a,b);
  
    return a+b+c;
}


