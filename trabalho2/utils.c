#include "utils.h"
#include <math.h>

// TROCAR OS LIMITES DO INTERVALO AQUI
#define leftLimit -100
#define rightLimit 100

#define FAKE_PROCESSING_LOOP_SIZE 100000

void fake_processing() {
    double x = FAKE_PROCESSING_LOOP_SIZE;
    for (int i=0; i< x; i++) {
        double k = x*x*x;
        double p = k*500.50005;
        double m = p/(x*x);
        k = m;
    }
}

double square (double x) {
    // fake_processing();
    return x*x;
}

double poly_func (double x) {
    // fake_processing();
    return x*x*x*x + 4*x;
}

double sinc_func (double x) {
    if (x==0)
        return 1;
    
    fake_processing();

    return fabs(sin(x)/x);
}

double (*get_func())(double) {

    // TROCAR A FUNÇÃO A SER TESTADA AQUI
    return &sinc_func;
}

double get_left_limit(){
    return leftLimit;
}

double get_right_limit(){
    return rightLimit;
}