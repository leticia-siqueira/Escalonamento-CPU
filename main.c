#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINHA 256
#define MAX_tarefas_arquivo 200

typedef struct escalonador{
    char nome[100];
    int periodo;
    int deadline;     
    int burst;          

    int tempo_sobrando;      
    int deadline_agora;      
    int esta_pronta;         

    int terminou;        
    int perdeu_prazo;             
} escalonador;

escalonador tarefas[MAX_tarefas_arquivo];
int tarefas_totais = 0;
int tempo_total = 0;

void ler_arquivo_entrada(const char *nome_arquivo){
    
    FILE *arquivo_de_entrada = fopen(nome_arquivo, "r");

    if(arquivo_de_entrada == NULL){
        fprintf(stderr, "erro ao abrir o arquivo\n");
        exit(1);
    }

    char linha[MAX_LINHA];

    if(fgets(linha, sizeof(linha), arquivo_de_entrada) == NULL){
        fprintf(stderr, "Erro de arquivo de entrada vazio, preencha corretamente o arquivo\n");
        fclose(arquivo_de_entrada);
        exit(1);
    }

    char *converter;
    tempo_total = strtol(linha, &converter, 10);

    if (converter == linha || tempo_total <= 0){
        fprintf(stderr, "Tempo total invalido, deve ser um numero positivo\n");
        fclose(arquivo_de_entrada);
        exit(1);
    }

    while(fgets(linha, sizeof(linha), arquivo_de_entrada) != NULL){
        
        char nome[100];
        int periodo, deadline, burst;
        char verifica_argumento_extra[MAX_LINHA];
        
        int campos_lidos = sscanf(linha, "%99s %d %d %d %s", nome, &periodo, &deadline, &burst, verifica_argumento_extra);
        
        if (campos_lidos != 4){
            fprintf(stderr, "Quantidade de argumentos da tarefa esta errado, use [nome] [periodo] [deadline] [burst]\n");
            fclose(arquivo_de_entrada);
            exit(1);
        }

        if (periodo <= 0 || deadline <= 0 || burst <= 0){
            fprintf(stderr, "Valor invalido do argumento, periodo deadline e burst precisam ser maiores que 0\n");
            fclose(arquivo_de_entrada);
            exit(1);
        }
        
        if (burst > deadline || deadline > periodo || periodo < burst){
            fprintf(stderr, "Violação da regra C <= D <= P, siga esse padrao para o programa funcionar\n");
            fclose(arquivo_de_entrada);
            exit(1);
        }

        if (tarefas_totais >= MAX_tarefas_arquivo){
            fprintf(stderr, "Numero de tarefas maior que o limite :(\n");
            fclose(arquivo_de_entrada);
            exit(1);
        }

        strcpy(tarefas[tarefas_totais].nome, nome);

        tarefas[tarefas_totais].periodo = periodo;
        tarefas[tarefas_totais].deadline = deadline;
        tarefas[tarefas_totais].burst = burst;

        tarefas[tarefas_totais].tempo_sobrando = 0;
        tarefas[tarefas_totais].deadline_agora = 0;
        tarefas[tarefas_totais].esta_pronta = 0;

        tarefas[tarefas_totais].terminou = 0;
        tarefas[tarefas_totais].perdeu_prazo = 0;

        tarefas_totais++;
    }

    fclose(arquivo_de_entrada);
}

void funcao_rate(escalonador tarefas[], int tarefas_totais, int tempo_total){

    FILE *arquivo_saida_rate = fopen("rate_lmss4.out", "w");

    if (arquivo_saida_rate == NULL){
        fprintf(stderr, "Erro ao abrir arquivo de arquivo_saida_rate do Rate\n");
        exit(1);
    }

    fprintf(arquivo_saida_rate, "EXECUTION BY RATE\n\n");

    int tarefa_atual = -1;
    int inicio = 0;

    for (int i = 0; i < tempo_total; i++){

        int perdeu_prazo_agora = 0;
        
        for (int a = 0; a < tarefas_totais; a++){
            
            if (tarefas[a].esta_pronta == 1 && tarefas[a].deadline_agora == i && tarefas[a].tempo_sobrando > 0){
                tarefas[a].perdeu_prazo++;
                tarefas[a].tempo_sobrando = 0;
                tarefas[a].esta_pronta = 0;

                if (a == tarefa_atual){
                    perdeu_prazo_agora = 1;
                }
            }
        }

        for (int j = 0; j < tarefas_totais; j++){
            if (i % tarefas[j].periodo == 0){
                tarefas[j].tempo_sobrando = tarefas[j].burst;
                tarefas[j].deadline_agora = i + tarefas[j].deadline;
                tarefas[j].esta_pronta = 1;
            }
        }


        int tarefa_priorizada = -1;

        for (int b = 0; b < tarefas_totais; b++){
            
            if (tarefas[b].esta_pronta == 1 && tarefas[b].tempo_sobrando > 0){
            
                if (tarefa_priorizada == -1 || tarefas[b].periodo < tarefas[tarefa_priorizada].periodo){
                    tarefa_priorizada = b;
                }
            }
        }

        if (tarefa_priorizada != tarefa_atual){

            int duracao = i - inicio;

            if (duracao > 0){
               
                if (tarefa_atual == -1){
                    fprintf(arquivo_saida_rate, "idle for %d units\n", duracao);
               
                }else{
                    char letra_estado_final;

                    if (perdeu_prazo_agora == 1){
                        letra_estado_final = 'L';
                    
                    }else if (tarefas[tarefa_atual].tempo_sobrando == 0){
                        letra_estado_final = 'F';
                    
                    }else{
                        letra_estado_final = 'H';
                    }

                    fprintf(arquivo_saida_rate, "[%s] for %d units - %c\n", tarefas[tarefa_atual].nome, duracao, letra_estado_final);
                }
            }

            tarefa_atual = tarefa_priorizada;
            inicio = i;
        }

        if (tarefa_priorizada != -1){
            
            tarefas[tarefa_priorizada].tempo_sobrando--;

            if (tarefas[tarefa_priorizada].tempo_sobrando == 0){
                tarefas[tarefa_priorizada].terminou++;
                tarefas[tarefa_priorizada].esta_pronta = 0;
            }
        }
    }

    int duracao_final = tempo_total - inicio;
    
    if (duracao_final > 0){

    if (tarefa_atual == -1){
        fprintf(arquivo_saida_rate, "idle for %d units\n", duracao_final);

    }else if (tarefas[tarefa_atual].tempo_sobrando == 0){
        
        fprintf(arquivo_saida_rate, "[%s] for %d units - F\n", tarefas[tarefa_atual].nome, duracao_final);

    }else{
        
        fprintf(arquivo_saida_rate, "[%s] for %d units\n", tarefas[tarefa_atual].nome, duracao_final);
    }
}

    fprintf(arquivo_saida_rate, "\nLOST DEADLINES\n\n");
    
    for (int k = 0; k < tarefas_totais; k++){
        fprintf(arquivo_saida_rate, "[%s] %d\n", tarefas[k].nome, tarefas[k].perdeu_prazo);
    }

    fprintf(arquivo_saida_rate, "\nCOMPLETE EXECUTION\n\n");
    
    for (int k = 0; k < tarefas_totais; k++){
        fprintf(arquivo_saida_rate, "[%s] %d\n", tarefas[k].nome, tarefas[k].terminou);
    }

    fprintf(arquivo_saida_rate, "\nKILLED\n\n");
    
    for (int k = 0; k < tarefas_totais; k++){
        int killed = 0;
    
        if (tarefas[k].esta_pronta == 1 && tarefas[k].tempo_sobrando > 0){
            killed = 1;
        }
    
        fprintf(arquivo_saida_rate, "[%s] %d\n", tarefas[k].nome, killed);
    }

    fclose(arquivo_saida_rate);
}

void funcao_edf(escalonador tarefas[], int tarefas_totais, int tempo_total){

    FILE *arquivo_saida_edf = fopen("edf_lmss4.out", "w");

    if (arquivo_saida_edf == NULL){
        fprintf(stderr, "Erro ao abrir arquivo de saida do EDF\n");
        exit(1);
    }

    fprintf(arquivo_saida_edf, "EXECUTION BY EDF\n\n");

    int tarefa_atual = -1;
    int inicio = 0;

    for (int i = 0; i < tempo_total; i++){

        int perdeu_prazo_agora = 0;
        
        for (int a = 0; a < tarefas_totais; a++){

            if (tarefas[a].esta_pronta == 1 && tarefas[a].deadline_agora == i && tarefas[a].tempo_sobrando > 0){
                tarefas[a].perdeu_prazo++;
                tarefas[a].tempo_sobrando = 0;
                tarefas[a].esta_pronta = 0;

                if (a == tarefa_atual){
                    perdeu_prazo_agora = 1;
                }
            }
        }

        for (int j = 0; j < tarefas_totais; j++){
            if (i % tarefas[j].periodo == 0){
                tarefas[j].tempo_sobrando = tarefas[j].burst;
                tarefas[j].deadline_agora = i + tarefas[j].deadline;
                tarefas[j].esta_pronta = 1;
            }
        }


        int tarefa_priorizada = -1;

        for (int b = 0; b < tarefas_totais; b++){

            if (tarefas[b].esta_pronta == 1 && tarefas[b].tempo_sobrando > 0){

                if (tarefa_priorizada == -1 || tarefas[b].deadline_agora < tarefas[tarefa_priorizada].deadline_agora){
                    tarefa_priorizada = b;
                }
            }
        }

        if (tarefa_priorizada != tarefa_atual){

            int duracao = i - inicio;

            if (duracao > 0){

                if (tarefa_atual == -1){
                    fprintf(arquivo_saida_edf, "idle for %d units\n", duracao);

                }else{
                    char letra_estado_final;

                    if (perdeu_prazo_agora == 1){
                        letra_estado_final = 'L';

                    }else if (tarefas[tarefa_atual].tempo_sobrando == 0){
                        letra_estado_final = 'F';

                    }else{
                        letra_estado_final = 'H';
                    }

                    fprintf(arquivo_saida_edf, "[%s] for %d units - %c\n", tarefas[tarefa_atual].nome, duracao, letra_estado_final);
                }
            }

            tarefa_atual = tarefa_priorizada;
            inicio = i;
        }

        if (tarefa_priorizada != -1){

            tarefas[tarefa_priorizada].tempo_sobrando--;

            if (tarefas[tarefa_priorizada].tempo_sobrando == 0){
                tarefas[tarefa_priorizada].terminou++;
                tarefas[tarefa_priorizada].esta_pronta = 0;
            }
        }
    }

    int duracao_final = tempo_total - inicio;

    if (duracao_final > 0){

    if (tarefa_atual == -1){
        fprintf(arquivo_saida_edf, "idle for %d units\n", duracao_final);

    }else if (tarefas[tarefa_atual].tempo_sobrando == 0){
    
        fprintf(arquivo_saida_edf, "[%s] for %d units - F\n", tarefas[tarefa_atual].nome, duracao_final);

    }else{
        
        fprintf(arquivo_saida_edf, "[%s] for %d units\n", tarefas[tarefa_atual].nome, duracao_final);
    }
}

    fprintf(arquivo_saida_edf, "\nLOST DEADLINES\n\n");

    for (int k = 0; k < tarefas_totais; k++){
        fprintf(arquivo_saida_edf, "[%s] %d\n", tarefas[k].nome, tarefas[k].perdeu_prazo);
    }

    fprintf(arquivo_saida_edf, "\nCOMPLETE EXECUTION\n\n");

    for (int k = 0; k < tarefas_totais; k++){
        fprintf(arquivo_saida_edf, "[%s] %d\n", tarefas[k].nome, tarefas[k].terminou);
    }

    fprintf(arquivo_saida_edf, "\nKILLED\n\n");

    for (int k = 0; k < tarefas_totais; k++){
        int killed = 0;

        if (tarefas[k].esta_pronta == 1 && tarefas[k].tempo_sobrando > 0){
            killed = 1;
        }

        fprintf(arquivo_saida_edf, "[%s] %d\n", tarefas[k].nome, killed);
    }

    fclose(arquivo_saida_edf);
}


int main(int argc, char *argv[]){

    if (argc != 3){
        fprintf(stderr, "Quantidade de argumentos errada. Formato certo ./scheduler [algoritmo] [voo.txt]\n");
        exit(1);
    }

    const char *nome_algoritmo = argv[1];
    const char *arquivo = argv[2];
    
    if(strcmp(nome_algoritmo, "rate") != 0 && strcmp(nome_algoritmo, "edf") != 0){
        fprintf(stderr, "Algoritmo invalido, use rate ou edf\n");
        exit(1);
    } 
    
    ler_arquivo_entrada(arquivo);
    
    if(strcmp(nome_algoritmo, "rate") == 0){
        funcao_rate(tarefas, tarefas_totais, tempo_total);
        
    }else if(strcmp(nome_algoritmo, "edf") == 0){
        funcao_edf(tarefas, tarefas_totais, tempo_total);

    }
    
    return 0;
}