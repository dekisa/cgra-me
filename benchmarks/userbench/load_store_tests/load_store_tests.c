int test_function(int ref_by_value, int* ref_by_pointer, int array[], int** array2d, long int index0, long int index1) {
    //DFGLoop:loop
    //access ref_by_value
    //load ref_by_pointer_array
    //store ref_by_pointer
    
    //just read the reference;
    int a = ref_by_value;

    //read the pointer
    int b =  *ref_by_pointer;

    //read the array
    int c = array[1];

    //read the array with input index
    int d = array[index0];

    //read 2d array
    int e = array2d[1][3] + array2d[1][2];

    //store to pointer
    *ref_by_pointer = a+b+c+d+e;

    //store to array
    array[2]= a+b+c+d+e;

    //store to array
    array[index1]= a+b+c+d+e;

    //store to 2d array
    array2d[4][3] = a+b+c+d+e;

    //return is output
    return a+b+c+d+e;
}

