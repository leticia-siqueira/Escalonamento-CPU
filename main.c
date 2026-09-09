#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINHA 256
#define MAX_tarefas_arquivo 200

typedef struct {
    char nome[64];
    int periodo;
    int deadline;     
    int burst;          

    int tempo_sobrando;      
    int deadline_agora;      
    int esta_pronta;         

    int terminou;        
    int perdeu_prazo;  
    int morreu;           
} Tarefa;

Tarefa tarefas[MAX_tarefas_arquivo];
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
        fprintf(stderr, "Erro de arquivo ou linha vazia, coloque as informacoes em linhas seguidas\n");
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
        
        int campos_lidos = sscanf(linha, "%99s %d %d %d", nome, &periodo, &deadline, &burst);
        
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
        tarefas[tarefas_totais].morreu = 0;

        tarefas_totais++;
    }

    fclose(arquivo_de_entrada);
}

void funcao_rate(Tarefa tarefas[], int tarefas_totais, int tempo_total){

    FILE *arquivo_saida_rate = fopen("rate_lmss4.out", "w");

    if (arquivo_saida_rate == NULL){
        fprintf(stderr, "Erro ao abrir arquivo de arquivo_saida_rate do Rate\n");
        exit(1);
    }

    int tarefa_atual = -1;   
    int inicio = 0;

    for (int i = 0; i < tempo_total; i++){

        for (int j = 0; j < tarefas_totais; j++){
            if (i % tarefas[j].periodo == 0){
                tarefas[j].tempo_sobrando = tarefas[j].burst;
                tarefas[j].deadline_agora = i + tarefas[j].deadline;
                tarefas[j].esta_pronta = 1;
            }
        }

        for (int a = 0; a < tarefas_totais; a++){
            if (tarefas[a].esta_pronta == 1 && tarefas[a].deadline_agora == i && tarefas[a].tempo_sobrando > 0){
                tarefas[a].perdeu_prazo++;
                tarefas[a].tempo_sobrando = 0;
                tarefas[a].esta_pronta = 0;
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
                char letra;
                
                if (perdeu_bloco_agora == 1){
                    letra = 'L';
                }else if (tarefas[tarefa_atual].tempo_sobrando == 0){
                    letra = 'F';
                }else{
                    letra = 'H';
                }
                fprintf(arquivo_saida_rate, "[%s] for %d units - %c\n", tarefas[tarefa_atual].nome, duracao, letra);
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

    fclose(arquivo_saida_rate);
}

void funcao_edf(Tarefa tarefas[], int tarefas_totais, int tempo_total){
    // loop  i = 0 ate tempo_total, prioridade = menor deadline_agora
}


int main(int argc, char *argv[]){

    if (argc != 3){
        fprintf(stderr, "Quantidade de argumentos errada. Formato certo ./scheduler [algoritmo] [voo.txt]");
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