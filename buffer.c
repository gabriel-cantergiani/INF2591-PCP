#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"
#include <fcntl.h>
#include <string.h>

struct sbuffer {
    sem_t mutex;
    sem_t s_producers;
    sem_t ** s_consumers;
    int producers_waiting;
    int * consumers_waiting;
    int * buffer; //buffer de N posicoes
    int * lidos; //array de tamanho C consumidores que indica quantos itens foram lidos por cada consumidor
    int * faltaler; //array de tamanho N que indica quantos consumidores faltam ler um item para cada posição do buffer
    int escritos; //variavel que indica a quantidade de itens já produzidos pelos produtores
    int prox_pos_escrita; //variavel que indica a próxima posição livre para ser escrita no buffer
    int * prox_pos_leitura; //array de tamanho C consumidores que indica qual a próxima posição a ser lida para cada consumidor (índice é o id do consumidor)
    int N; //variavel que indica o número de posição no buffer
    int C; //variavel que indica o número de consumidores
};

char * nameRandomize (char * name, int nLetters) {
  int i, nameSize;
  char * randomName;
  nameSize = strlen(name);
  randomName = (char *) malloc ((nameSize + nLetters + 1) * sizeof(char));
  if (randomName == NULL) {
    printf("Error during randomName alloc\n");
    exit(0);
  }

  strcpy(randomName, name);
  for(i=0;i<nLetters;i++){
    randomName[nameSize + i] = 'A' + rand()%26;
  }
  randomName[nameSize + i] = '\0'; // Add '\0' at the end so it is considered as a string

  return randomName;
}

tbuffer* iniciabuffer (int numpos, int numprod, int numcons){

    // Allocate memory for buffer
    tbuffer * buf = (tbuffer*) malloc(sizeof(tbuffer));
    buf->buffer = (int *) malloc(sizeof(int) * numpos);
    buf->lidos = (int *) malloc(sizeof(int) * numcons);
    buf->faltaler = (int *) malloc(sizeof(int) * numpos);
    buf->prox_pos_leitura = (int *) malloc(sizeof(int) * numcons);
    buf->consumers_waiting = (int *) malloc(sizeof(int) * numcons);
    buf->s_consumers = (sem_t **) malloc(sizeof(sem_t) * numcons);

    // Initialize variables
    buf->escritos = 0;
    buf->prox_pos_escrita = 0;
    buf->N = numpos;
    buf->C = numcons;
    buf->producers_waiting = 0;

    for(int i=0; i<numpos; i++){
        buf->buffer[i] = 0;
        buf->faltaler[i] = 0;
    }

    for(int i=0; i<numcons; i++){
        buf->lidos[i] = 0;
        buf->prox_pos_leitura[i] = 0;
        buf->consumers_waiting[i] = 0;
        // sem_t sem = buf->s_consumers[i];
        // sem_init(&sem, 0, 0);
        // sem_init(&sem, 0, 0);
        // buf->s_consumers[i] = sem;
        // int length = snprintf( NULL, 0, "%d", i );
        // char* str = malloc( length + 1 );
        // snprintf( str, length + 1, "%d", i );
        // printf("SEM NAME: %s", str);
        char * semaphoreName;
        semaphoreName = nameRandomize ("/semmC", 20 + i);
        sem_unlink(semaphoreName);
        buf->s_consumers[i] = sem_open(semaphoreName, O_CREAT|O_EXCL, S_IRUSR | S_IWUSR, 0);
        if (buf->s_consumers[i] == SEM_FAILED){
            printf("Error opening semaphore semCons[%d].\n", i);
            exit(0);
        }
    }

    // Initialize semaphores
    sem_init(&buf->mutex, 0, 1);
    sem_init(&buf->s_producers, 0, 0);

    return buf;
}

void deposita (tbuffer* buffer, int item){

    sem_wait(&buffer->mutex);    
    printf("[PRODUCER] Producer producing...\n");
    
    int index = buffer->prox_pos_escrita;

    if(buffer->faltaler[index] > 0){
        buffer->producers_waiting++;
        sem_post(&buffer->mutex);
        printf("[PRODUCER] Producer waiting for baton...\n");
        sem_wait(&buffer->s_producers);
        printf("[PRODUCER] Producer received baton!\n");
        buffer->producers_waiting--;
    }

    buffer->buffer[index] = item;
    buffer->faltaler[index] = buffer->C;
    buffer->escritos++;
    buffer->prox_pos_escrita = (buffer->prox_pos_escrita + 1) % buffer->N;
    
    printf("[PRODUCER] Producer wrote on buffer[prox_pos_escrita] = item: %d\n", buffer->buffer[index]);
    // printf("[PRODUCER] faltaler[prox_pos_escrita] = C: %d\n", buffer->faltaler[index]);
    // printf("[PRODUCER] escritos++: %d\n", buffer->escritos);
    // printf("[PRODUCER] prox_pos_escrita: %d\n", buffer->prox_pos_escrita);

    for(int i=0; i<buffer->C; i++){
        if(buffer->consumers_waiting[i] && buffer->escritos > buffer->lidos[i]){
            printf("[PRODUCER] Producer passing baton to consumer %d\n", i);
            sem_post(buffer->s_consumers[i]);
            sleep(1);
            return;
        }
    }

    if (buffer->producers_waiting > 0){
        printf("[PRODUCER] Producer passing baton to other producer\n");
        sem_post(&buffer->s_producers);
    }
    else {
        printf("[PRODUCER] Producer releasing baton\n");
        sem_post(&buffer->mutex);
    }

    sleep(1);
    return;
}

int consome (tbuffer* buffer, int meuid){

    sem_wait(&buffer->mutex);
    printf("[CONSUMER %d] Consumer consuming...\n", meuid);
    
    int index = buffer->prox_pos_leitura[meuid];
    int item;

    if(buffer->lidos[meuid] >= buffer->escritos){
        printf("[CONSUMER %d] Consumer waiting for baton...\n", meuid);
        buffer->consumers_waiting[meuid] = 1;
        sem_post(&buffer->mutex);
        sem_wait(buffer->s_consumers[meuid]);
        printf("[CONSUMER %d] Consumer received baton!\n", meuid);
        buffer->consumers_waiting[meuid] = 0;
    }

    item = buffer->buffer[index];
    buffer->faltaler[index]--;
    buffer->lidos[meuid]++;
    buffer->prox_pos_leitura[meuid] = (buffer->prox_pos_leitura[meuid] + 1) % buffer->N;

    printf("[CONSUMER %d] Consumer read from buffer item: %d\n", meuid, buffer->buffer[index]);
    // printf("[CONSUMER %d] faltaler: %d\n", meuid, buffer->faltaler[index]);
    // printf("[CONSUMER %d] lidos[meuid]++: %d\n", meuid, buffer->lidos[meuid]);
    // printf("[CONSUMER %d] prox_pos_leitura: %d\n", meuid, buffer->prox_pos_leitura[meuid]);

    for(int i=0; i<buffer->C; i++){
        if(buffer->consumers_waiting[i] && buffer->escritos > buffer->lidos[i]){
            printf("[CONSUMER %d] Consumer passing baton to other consumer %d\n", meuid, i);
            sem_post(buffer->s_consumers[i]);
            sleep(2);
            return item;
        }
    }

    if (buffer->faltaler[buffer->prox_pos_escrita] == 0 && buffer->producers_waiting > 0){
        printf("[CONSUMER %d] Consumer passing baton to producer\n", meuid);
        sem_post(&buffer->s_producers);
    }
    else {
        printf("[CONSUMER %d] Consumer releasing mutex\n", meuid);
        sem_post(&buffer->mutex);
    }

    sleep(2);
    return item;
}

void finalizabuffer (tbuffer* buffer){

    free(buffer->buffer);
    free(buffer->lidos);
    free(buffer->faltaler);
    free(buffer->prox_pos_leitura);
    free(buffer);

    return;
}