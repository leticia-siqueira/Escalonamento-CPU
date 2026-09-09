# Escalonamento-CPU

Esta atividade propõe a implementação de um simulador de escalonamento preemptivo de tarefas periódicas de tempo real crítico para sistemas de computador de bordo. O objetivo principal é comparar o comportamento de dois algoritmos clássicos: Rate-Monotonic (rate) e Earliest-Deadline-First (edf).

Para compilar:

Rode: 

make clean

make

e depois para rodar o escalonador Rate:

./scheduler rate nome_do_arquivo.txt 

Para rodar o escalonador Edf:

./scheduler rate nome_do_arquivo.txt 

(criei um arquivo base voo.txt, mas seu uso é opcional na hora de executar pode apagar ele e criar outro de sua preferência)
