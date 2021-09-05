#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"

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
    }

    // Initialize semaphore
    sem_init(&buf->mutex, 0, 1);
    sem_init(&buf->prod, 0, 0);

    buf->cons = (sem_t *) malloc(sizeof(sem_t) * numcons);
    if(buf->cons == NULL) {
        printf("Error in cons' alloc\n");
    }

    for(int i=0; i<numcons; i++)
        sem_init(&buf->cons[i], 0, 0);

    return buf;
}

void deposita (tbuffer* buffer, int item){
    while (1) {
        sem_wait(&buffer->mutex);    
        printf("[PRODUCER] Producer producing...\n");
        
        int index = buffer->prox_pos_escrita;

        if(buffer->faltaler[index] > 0){ //ainda existem consumidores para consumir (algum consumidor não consumiu o item daquela posição ainda)
            buffer->prodEsperando++;
            sem_post(&buffer->mutex); //produtor liberar o semáforo
            sem_wait(&buffer->prod); //e se coloca em espera
        }

        if(buffer->faltaler[index] == 0){
            buffer->buffer[index] = item;
            buffer->faltaler[index] = buffer->C;
            buffer->escritos++;
            buffer->prox_pos_escrita = (buffer->prox_pos_escrita + 1) % buffer->N;
            index = buffer->prox_pos_escrita;
            
            printf("[PRODUCER] buffer[prox_pos_escrita] = item: %d\n", buffer->buffer[index]);
            printf("[PRODUCER] faltaler[prox_pos_escrita] = C: %d\n", buffer->faltaler[index]);
            printf("[PRODUCER] escritos++: %d\n", buffer->escritos);
            printf("[PRODUCER] prox_pos_escrita: %d\n", buffer->prox_pos_escrita);
        }

        for(int c=0; c<buffer->C; c++){ //depois de  produzir o item, o produtor checa se tem algum consumidor esperando
            if(buffer->lidos[c] < buffer->escritos){
                sem_post(&buffer->cons[c]); //passa o bastão para o primeiro consumidor que estiver esperando                
                return;
            }
        }

        //se não tem consumidor esperando, ele checa se todos os consumidores já leram a próxima posição e se tem algum produtor esperando para produzir
        if(buffer->faltaler[index] == 0 && buffer->prodEsperando > 0){
            buffer->prodEsperando--;
            sem_post(&buffer->prod);
        }
        else{
            sem_post(&buffer->mutex);
        }

        //rever
        if(buffer->faltaler[index] == 0 && buffer->prodEsperando == 0)
            return;
    }
}

int consome (tbuffer* buffer, int meuid){
    while (1) {
        sem_wait(&buffer->mutex);
        printf("[CONSUMER %d] Consumer consuming...\n", meuid);
        
        int index = buffer->prox_pos_leitura[meuid];
        int item;

        if(buffer->faltaler[index] == 0){ //não existem itens a serem consumidos ainda
            sem_post(&buffer->mutex);
            sem_wait(&buffer->cons[meuid]); //esperando até que produtor libere este consumidor para ler
        }

        if(buffer->faltaler[index] > 0 && buffer->lidos[meuid] < buffer->escritos){
            item = buffer->buffer[index];
            buffer->faltaler[index]--;
            buffer->lidos[meuid]++;
            buffer->prox_pos_leitura[meuid] = (buffer->prox_pos_leitura[meuid] + 1) % buffer->N;
            index = buffer->prox_pos_leitura[meuid];

            printf("[CONSUMER %d] item: %d\n", meuid, buffer->buffer[index]);
            printf("[CONSUMER %d] faltaler: %d\n", meuid, buffer->faltaler[index]);
            printf("[CONSUMER %d] lidos[meuid]++: %d\n", meuid, buffer->lidos[meuid]);
            printf("[CONSUMER %d] prox_pos_leitura: %d\n", meuid, buffer->prox_pos_leitura[meuid]);
        }

        for(int c=0; c<buffer->C; c++){ //depois de  consumir o item, o consumidor checa se tem algum consumidor esperando
            if(buffer->lidos[c] < buffer->escritos){
                sem_post(&buffer->cons[c]); //passa o bastão para o primeiro consumidor que estiver esperando
                return 1;
            }
        }

        //se não tem consumidor esperando, ele checa se todos os consumidores já leram a próxima posição e se tem algum produtor esperando para produzir
        if(buffer->faltaler[index] == 0 && buffer->prodEsperando > 0){
            buffer->prodEsperando--;
            sem_post(&buffer->prod);
        }
        else{
            sem_post(&buffer->mutex);
        }

        if(buffer->faltaler[index] == 0 && buffer->lidos[meuid] == buffer->escritos)
            break;
    }
}

void finalizabuffer (tbuffer* buffer){

    free(buffer->buffer);
    free(buffer->lidos);
    free(buffer->faltaler);
    free(buffer->prox_pos_leitura);
    free(buffer);

    return;
}