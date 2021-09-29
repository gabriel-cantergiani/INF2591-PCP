#include "utils.h"
#include <math.h>

#define leftLimit -10
#define rightLimit 10

#define FAKE_PROCESSING_LOOP_SIZE 10000000

void fake_processing(double x) {
    for (int i=0; i< x; i++) {
        double k = x*x*x;
        double p = k*500.50005;
        double m = p/(x*x);
        k = m;
    }
}

double square (double x) {
    // fake_processing(FAKE_PROCESSING_LOOP_SIZE);
    return x*x;
}

double poly_func (double x) {
    // fake_processing(FAKE_PROCESSING_LOOP_SIZE);
    return x*x*x*x + 4*x;
}

double sinc_func (double x) {
    if (x==0)
        return 1;

    return fabs(sin(x)/x);
}

double (*get_func())(double) {

    // TROCAR A FUNÇÃO TESTADA AQUI
    
    return &sinc_func;
}

double get_left_limit(){
    return leftLimit;
}

double get_right_limit(){
    return rightLimit;
}