#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"
#include <fcntl.h>
#include <string.h>

struct sbuffer {
    sem_t mutex;
    sem_t prod;
    sem_t * cons;

    int * buffer; //buffer de N posicoes
    int * lidos; //array de tamanho C consumidores que indica quantos itens foram lidos por cada consumidor
    int * faltaler; //array de tamanho N que indica quantos consumidores faltam ler um item para cada posição do buffer
    int escritos; //variavel que indica a quantidade de itens já produzidos pelos produtores
    int prox_pos_escrita; //variavel que indica a próxima posição livre para ser escrita no buffer
    int * prox_pos_leitura; //array de tamanho C consumidores que indica qual a próxima posição a ser lida para cada consumidor (índice é o id do consumidor)
    int N; //variavel que indica o número de posição no buffer
    int C; //variavel que indica o número de consumidores

    int prodEsperando; //variavel que indica a quantidade de produtores esperando para escrever no buffer
    int * consEsperando; //array de tamanho C consumidores que indica se o consumidor de cada índice está esperando pelo bastao
};

tbuffer* iniciabuffer (int numpos, int numprod, int numcons){

    // Allocate memory for buffer
    tbuffer * buf = (tbuffer*) malloc(sizeof(tbuffer));
    if(buf == NULL) {
        printf("Error in buf's alloc\n");
    }

    buf->buffer = (int *) malloc(sizeof(int) * numpos);
    if(buf->buffer == NULL) {
        printf("Error in buffer's alloc\n");
    }

    buf->lidos = (int *) malloc(sizeof(int) * numcons);
    if(buf->lidos == NULL) {
        printf("Error in lidos' alloc\n");
    }
    
    buf->faltaler = (int *) malloc(sizeof(int) * numpos);
    if(buf->faltaler == NULL) {
        printf("Error in faltaler's alloc\n");
    }

    buf->prox_pos_leitura = (int *) malloc(sizeof(int) * numcons);
    if(buf->prox_pos_leitura == NULL) {
        printf("Error in prox_pos_leitura's alloc\n");
    }

    buf->consEsperando = (int *) malloc(sizeof(int) * numcons);
    if(buf->consEsperando == NULL) {
        printf("Error in consEsperando's alloc\n");
    }

    buf->cons = (sem_t *) malloc(sizeof(sem_t) * numcons);
    if(buf->cons == NULL) {
        printf("Error in cons' alloc\n");
    }

    // Initialize variables
    buf->escritos = 0;
    buf->prox_pos_escrita = 0;
    buf->N = numpos;
    buf->C = numcons;
    buf->prodEsperando = 0;

    for(int i=0; i<numpos; i++){
        buf->buffer[i] = 0;
        buf->faltaler[i] = 0;
    }

    for(int i=0; i<numcons; i++){
        buf->lidos[i] = 0;
        buf->prox_pos_leitura[i] = 0;
        buf->consEsperando[i] = 0;
        int res = sem_init(&buf->cons[i], 0, 0);
        if (res == -1) {
            printf("Error creating semaphore for cons[%d]", i);
            exit(1);
        }
    }

    // Initialize semaphores
    sem_init(&buf->mutex, 0, 1);
    sem_init(&buf->prod, 0, 0);

    return buf;
}

void deposita (tbuffer* buffer, int item){

    sem_wait(&buffer->mutex);    
    printf("[PRODUCER] Producer producing...\n");
    
    int index = buffer->prox_pos_escrita;

    if(buffer->faltaler[index] > 0){
        buffer->prodEsperando++;
        sem_post(&buffer->mutex);
        printf("[PRODUCER] Producer waiting for baton...\n");
        sem_wait(&buffer->prod);
        printf("[PRODUCER] Producer received baton!\n");
        buffer->prodEsperando--;
    }

    buffer->buffer[index] = item;
    buffer->faltaler[index] = buffer->C;
    buffer->escritos++;
    buffer->prox_pos_escrita = (buffer->prox_pos_escrita + 1) % buffer->N;
    
    printf("[PRODUCER] Producer wrote on buffer[%d] = item: %d\n", index, buffer->buffer[index]);
    // printf("[PRODUCER] faltaler[prox_pos_escrita] = C: %d\n", buffer->faltaler[index]);
    // printf("[PRODUCER] escritos++: %d\n", buffer->escritos);
    // printf("[PRODUCER] prox_pos_escrita: %d\n", buffer->prox_pos_escrita);
    sleep(1);

    for(int i=0; i<buffer->C; i++){
        if(buffer->consEsperando[i] && buffer->escritos > buffer->lidos[i]){
            printf("[PRODUCER] Producer passing baton to consumer %d\n", i);
            sem_post(&buffer->cons[i]);
            return;
        }
    }

    if (buffer->prodEsperando > 0){
        printf("[PRODUCER] Producer passing baton to other producer\n");
        sem_post(&buffer->prod);
    }
    else {
        printf("[PRODUCER] Producer releasing baton\n");
        sem_post(&buffer->mutex);
    }

    return;
}

int consome (tbuffer* buffer, int meuid){

    sem_wait(&buffer->mutex);
    printf("[CONSUMER %d] Consumer consuming...\n", meuid);
    
    int index = buffer->prox_pos_leitura[meuid];
    int item;

    if(buffer->lidos[meuid] >= buffer->escritos){
        printf("[CONSUMER %d] Consumer waiting for baton...\n", meuid);
        buffer->consEsperando[meuid] = 1;
        sem_post(&buffer->mutex);
        sem_wait(&buffer->cons[meuid]);
        printf("[CONSUMER %d] Consumer received baton!\n", meuid);
        buffer->consEsperando[meuid] = 0;
    }

    item = buffer->buffer[index];
    buffer->faltaler[index]--;
    buffer->lidos[meuid]++;
    buffer->prox_pos_leitura[meuid] = (buffer->prox_pos_leitura[meuid] + 1) % buffer->N;

    printf("[CONSUMER %d] Consumer read from buffer[%d] item: %d\n", meuid, index, buffer->buffer[index]);
    // printf("[CONSUMER %d] faltaler: %d\n", meuid, buffer->faltaler[index]);
    // printf("[CONSUMER %d] lidos[meuid]++: %d\n", meuid, buffer->lidos[meuid]);
    // printf("[CONSUMER %d] prox_pos_leitura: %d\n", meuid, buffer->prox_pos_leitura[meuid]);
    sleep(2);

    for(int i=0; i<buffer->C; i++){
        if(buffer->consEsperando[i] && buffer->escritos > buffer->lidos[i]){
            printf("[CONSUMER %d] Consumer passing baton to other consumer %d\n", meuid, i);
            sem_post(&buffer->cons[i]);
            return item;
        }
    }

    if (buffer->faltaler[buffer->prox_pos_escrita] == 0 && buffer->prodEsperando > 0){
        printf("[CONSUMER %d] Consumer passing baton to producer\n", meuid);
        sem_post(&buffer->prod);
    }
    else {
        printf("[CONSUMER %d] Consumer releasing mutex\n", meuid);
        sem_post(&buffer->mutex);
    }

    return item;
}

void finalizabuffer (tbuffer* buffer){

    free(buffer->buffer);
    free(buffer->lidos);
    free(buffer->faltaler);
    free(buffer->prox_pos_leitura);
    free(buffer->cons);
    free(buffer->consEsperando);
    free(buffer);

    return;
}