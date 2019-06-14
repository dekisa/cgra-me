//#include <stdio.h>
//
//volatile int * a;//[N] = {1,2,3,4,5,6,7,8,9,10};
//
//volatile int * n;
// Simple accumulation
//int main() {
//
//    int N = *n;
//    int sum = 0;
//    int i;
//
//    for (i = 0; i < N; i++) {
//	sum += a[i];
//    }
//    printf("sum = %d\n", sum);
//
//    return sum;
//}

#include <stdio.h>

volatile int * a;//[10] = {1,2,3,4,5,6,7,8,9,10};

volatile int * n;
// Simple accumulation
int main() {

    int N = *n;
    int sum = 0;
    int i;
    volatile int input;

    for (i = 0; i < N; i++) { //ovde je bilo 10 da bi mogao unroll
//	if (i==0){		//ovo je sluzio lo da zove smo u prvoj iteraciji
        //DFGLoop:loop
//	}
        
//        if (i == 2)
            sum += N;
//        else
         //  sum += a[i];	//ovo je bilo da ne bi optimizovao
           i+=sum;
    }
    printf("sum = %d\n", sum);

    return sum;
}

