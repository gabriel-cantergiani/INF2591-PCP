#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"

struct sbuffer {
    sem_t mutex;
    sem_t empty;
    sem_t full;
    int * buffer; //buffer de N posicoes
    int * lidos; //array de tamanho C consumidores que indica quantos itens foram lidos por cada consumidor
    int * faltaler; //array de tamanho N que indica quantos consumidores faltam ler um item para cada posição do buffer
    int escritos; //variavel que indica a quantidade de itens já produzidos pelos produtores
    int prox_pos_escrita; //variavel que indica a próxima posição livre para ser escrita no buffer
    int * prox_pos_leitura; //array de tamanho C consumidores que indica qual a próxima posição a ser lida para cada consumidor (índice é o id do consumidor)
    int N; //variavel que indica o número de posição no buffer
    int C; //variavel que indica o número de consumidores
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
    buf->N = numpos;
    buf->C = numcons;

    for(int i=0; i<numpos; i++){
        buf->buffer[i] = 0;
        buf->faltaler[i] = 0;
    }

    for(int i=0; i<numcons; i++){
        buf->lidos[i] = 0;
        buf->prox_pos_leitura[i] = 0;
    }

    // Initialize semaphore
    sem_init(&buf->mutex, 0, 1);
    sem_init(&buf->empty, 0, numpos);
    sem_init(&buf->full, 0, 0);

    return buf;
}

void deposita (tbuffer* buffer, int item){
    sem_wait(&buffer->empty);
    sem_wait(&buffer->mutex);    
    printf("[PRODUCER] Producer producing...\n");
    
    int index = buffer->prox_pos_escrita;

    if(buffer->faltaler[index] == 0){
        buffer->buffer[index] = item;
        buffer->faltaler[index] = buffer->C;
        buffer->escritos++;
        buffer->prox_pos_escrita = (buffer->prox_pos_escrita + 1) % buffer->N;
        
        printf("[PRODUCER] buffer[prox_pos_escrita] = item: %d\n", buffer->buffer[index]);
        printf("[PRODUCER] faltaler[prox_pos_escrita] = C: %d\n", buffer->faltaler[index]);
        printf("[PRODUCER] escritos++: %d\n", buffer->escritos);
        printf("[PRODUCER] prox_pos_escrita: %d\n", buffer->prox_pos_escrita);
    }

    sem_post(&buffer->mutex);
    for(int i=0; i<buffer->C; i++)
        sem_post(&buffer->full);
    sleep(1);
    return;
}

int consome (tbuffer* buffer, int meuid){
    sem_wait(&buffer->full);
    sem_wait(&buffer->mutex);
    printf("[CONSUMER %d] Consumer consuming...\n", meuid);
    
    int index = buffer->prox_pos_leitura[meuid];
    int item;

    if(buffer->faltaler[index] > 0 && buffer->lidos[meuid] < buffer->escritos){
        item = buffer->buffer[index];
        buffer->faltaler[index]--;
        buffer->lidos[meuid]++;
        buffer->prox_pos_leitura[meuid] = (buffer->prox_pos_leitura[meuid] + 1) % buffer->N;

        printf("[CONSUMER %d] item: %d\n", meuid, buffer->buffer[index]);
        printf("[CONSUMER %d] faltaler: %d\n", meuid, buffer->faltaler[index]);
        printf("[CONSUMER %d] lidos[meuid]++: %d\n", meuid, buffer->lidos[meuid]);
        printf("[CONSUMER %d] prox_pos_leitura: %d\n", meuid, buffer->prox_pos_leitura[meuid]);
    }

    sem_post(&buffer->mutex);
    sem_post(&buffer->empty);
    sleep(2);
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