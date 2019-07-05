#include <stdio.h>
int** nglobal;
//int* npointer;
/*
load and store conversion to input and output
load_conversion_algorithm()
    ignore gep
    ignore load if its only used by gep
        if(is aray)
            if(index is constant)
            else if(index is function argument)
            else NOT DONE
                the same stuff again for 2d array
            if(pointer is function argument)
            else NOT DONE
        else
            NOT DONE
similar algorithm for store
*/

/* this is still just a demo, there is no convention on marking inputs and outputs altough the code is getting larger */

int test_function(int a, int** n, int* npointer) {

    //demonstrate store - has two outputs for now
    //return n[2][2]  = a + 10;
    
    //demonstrate load
    //return  (n[2][2] + n[3][1])*(n[2][2] - n[3][1]);
    
    //demonstrate load problem1 - index is long - enable demonstrate store for crash
    //return  (a + n[3][1])*(n[2][2] - n[3][1]);

    //demonstrate load problem2
    //return  (n[2][2] + n[2][1])*(n[2][2] - n[2][1]);

    //demonstrate undone work
    //return  (nglobal[2][2] + nglobal[3][1])*(nglobal[2][2] - nglobal[3][1]);
    
    //demonstrate undone work
    return *npointer;
    
}

