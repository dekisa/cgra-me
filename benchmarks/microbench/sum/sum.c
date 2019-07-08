#include <stdio.h>

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

int test_function(int ref_by_value, int* ref_by_pointer, int ref_by_pointer_array[]) {

    //DFGLoop:loop
    //access ref_by_value
    //load ref_by_pointer_array
    //store ref_by_pointer

    //*ref_by_pointer = ref_by_value + 10;;
    //return *ref_by_pointer = *ref_by_pointer + 1;

    ref_by_pointer_array[3] = ref_by_pointer_array[4] + ref_by_value + 10;;
    ref_by_pointer_array[4] = ref_by_value + 101;;
    return ref_by_pointer_array[2] = ref_by_pointer_array[3] + 1;
//    return *ref_by_pointer = ref_by_value + ref_by_pointer_array[2];
    
    //demonstrate load basic
    //return  (n[2] + n[3])*(n[2] - n[3]);

    //demonstrate load with argument, only long is supported
    //return  (n[2] + n[3])*(n[a+1]);

    //demonstrate load with pointer
    //return npointer;

}

