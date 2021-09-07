# INF2591-PCP

Este projeto consiste em uma implementação do problema de broadcast entre múltiplos produtores e múltiplos consumidores. Neste cenário específico, é utilizado um buffer de tamanho limitado, e é utilizada a técnica de passagem de bastão para controlar a entrada na seção crítica.

O tamanho do buffer, o número de produtores, o número de consumidores e o número total de itens produzidos são todos parâmetros passados como argumentos na linha de comando.

É possível acompanhar a execução do programa através de logs na saída padrão (prints). Para facilitar o acompanhamento do algoritmo, as operações de produção e consumo possuem um delay de 1 segundo.

Foram realizados diversos testes com diferentes parâmetros. Os resultados destes testes foram documentados no arquivo ```testes/inf2591_trab1testes.txt``` e os outputs de cada teste estão nos arquivos ```teste/testeX.txt```.

A seguir, instruções de uso do programa:

- Para compilar o programa, usando o gcc:
```bash
$ make compile
ou
$ gcc -pthread -o main main.c buffer.c producer_consumer.c
```

- Para executar o programa:
```bash
$ ./main <tam_buffer> <num_prod> <num_cons> <num_itens>
```
Onde:
```<tam_buffer>``` é o tamanho do buffer.
```<num_prod>``` é o número de produtores.
```<num_cons>``` é o número de consumidores.
```<num_itens>``` é o número total de itens produzidos por cada produtor.

Exemplo:
```bash
$ ./main 5 3 3 10
```