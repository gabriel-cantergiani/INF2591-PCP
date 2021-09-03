#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"

struct sbuffer {
    sem_t sem;
    int * buffer; //buffer de N posicoes
    int * lidos; //array de tamanho C consumidores que indica quantos itens foram lidos por cada consumidor
    int * faltaler; //array de tamanho N que indica quantos consumidores faltam ler um item para cada posição do buffer
    int escritos; //variavel que indica a quantidade de itens já produzidos pelos produtores
    int prox_pos_escrita; //variavel que indica a próxima posição livre para ser escrita no buffer
    int * prox_pos_leitura; //array de tamanho C consumidores que indica qual a próxima posição a ser lida para cada consumidor (índice é o id do consumidor)
};

tbuffer* iniciabuffer (int numpos, int numprod, int numcons){

    // Allocate memory for buffer
    tbuffer * buf = (tbuffer*) malloc(sizeof(tbuffer));
    buf->buffer = (int *) malloc(sizeof(int) * numpos);
    buf->lidos = (int *) malloc(sizeof(int) * numcons);
    buf->faltaler = (int *) malloc(sizeof(int) * numpos);
    buf->prox_pos_leitura = (int *) malloc(sizeof(int) * numcons);

    // Initialize variables
    buf->escritos = 0;
    buf->prox_pos_escrita = 0;

    for(int i=0; i<numpos; i++)
        buf->faltaler[i] = 0;

    for(int i=0; i<numcons; i++){
        buf->lidos[i] = 0;
        buf->prox_pos_leitura[i] = 0;
    }

    // Initialize semaphore
    sem_init(&buf->sem, 0, 1);

    return buf;
}

void deposita (tbuffer* buffer, int item){
    sem_wait(&buffer->sem);
    printf("Producer producing...\n");
    sleep(5);
    sem_post(&buffer->sem);
    return;
}

int consome (tbuffer* buffer, int meuid){
    sem_wait(&buffer->sem);
    printf("Consumer consuming...\n");
    sleep(5);
    sem_post(&buffer->sem);
    return 1;
}

void finalizabuffer (tbuffer* buffer){

    free(buffer->buffer);
    free(buffer->lidos);
    free(buffer->faltaler);
    free(buffer->prox_pos_leitura);
    free(buffer);

    return;
}