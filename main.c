#include <stdio.h>
#include "buffer.h"


int main(void) {
    tbuffer * buf = iniciabuffer(0, 0, 0);
    printf("Teste Consumo: %d\n", consome(buf, 0));
    return 0;
}