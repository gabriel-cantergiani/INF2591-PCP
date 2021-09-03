INF2591
Professora: Noemi Rodriguez
Grupo: Gabriel Cantergiani
       Mariana Salgueiro

Modelagem do Problema de Broadcast

int buffer[N] = {0,..,0}; //buffer de N posicoes
int lidos[C] = {0,..,0}; //array de tamanho C consumidores que indica quantos itens foram lidos por cada consumidor
int faltaler[N] = {0,..,0}; //array de tamanho N que indica quantos consumidores faltam ler um item para cada posição do buffer
int escritos = 0; //variavel que indica a quantidade de itens já produzidos pelos produtores
int prox_pos_escrita = 0; //variavel que indica a próxima posição livre para ser escrita no buffer
int prox_pos_leitura[C] = {0,..,0}; //array de tamanho C consumidores que indica qual a próxima posição a ser lida para cada consumidor (índice é o id do consumidor)

process Deposita(buffer, item){
  ...
  <await(faltaler[prox_pos_escrita] == 0);
  buffer[prox_pos_escrita] = item;
  faltaler[prox_pos_escrita] = C;
  escritos++;
  prox_pos_escrita = (prox_pos_escrita + 1)%N;>
  ...
}

process Consome(buffer, meuid){
  ...
  <await(faltaler[prox_pos_leitura[meuid]] > 0 && lidos[meuid] < escritos);
  item = buffer[prox_pos_leitura[meuid]];
  faltaler[prox_pos_leitura[meuid]]--;
  lidos[meuid]++;
  prox_pos_leitura[meuid] = (prox_pos_leitura[meuid] + 1)%N;>
  ...
}