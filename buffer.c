#include "buffer.h"
#include <stdlib.h>

struct sbuffer {};

tbuffer* iniciabuffer (int numpos, int numprod, int numcons){

    tbuffer * buf = (tbuffer*) malloc(sizeof(tbuffer));

    return buf;
}

void deposita (tbuffer* buffer, int item){
    return;
}

int consome (tbuffer* buffer, int meuid){
    return 1;
}

void finalizabuffer (tbuffer* buffer){
    return;
}