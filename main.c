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
    int tempo_total = strtol(linha, &converter, 10);

    if (fim_conversao == linha || tempo_total <= 0){
        fprintf(stderr, "Tempo total invalido, deve ser um numero positivo\n");
        fclose(arquivo_de_entrada);
        exit(1);
    }

    Tarefa tarefas[MAX_tarefas_arquivo];
    int tarefas_totais = 0;

    while(fgets(linha, sizeof(linha), arquivo_de_entrada) != NULL){
        
        char nome[64];
        int periodo, deadline, burst;
        
        int campos_lidos = sscanf(linha, "%s %d %d %d", nome, &periodo, &deadline, &burst);
        
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

        if (tarefas_totais >= MAX_TAREFAS){
            fprintf(stderr, "Numero de tarefas maior que o limite :(\n");
            fclose(arquivo_de_entrada);
            exit(1);
        }

        strcpy(tarefas[tarefas_totais].nome, nome);

        tarefas[tarefas_totais].periodo = periodo;
        tarefas[tarefas_totais].deadline = deadline;
        tarefas[tarefas_totais].burst = burst;

        tarefas[tarefas_totais].tempo_sobrando = 0;
        tarefas[tarefas_totais].deadline_atual = 0;
        tarefas[tarefas_totais].esta_pronta = 0;

        tarefas[tarefas_totais].terminou = 0;
        tarefas[tarefas_totais].perdeu_prazo = 0;
        tarefas[tarefas_totais].morreu = 0;

        tarefas_totais++;
    }

    fclose(arquivo_de_entrada);
}

int main(int argc, char *argv[]){

    if (argc != 3){
        fprintf(stderr, "Quantidade de argumentos errada. Formato certo ./scheduler [algoritmo] [voo.txt]");
        exit(1);
    }

    const char *nome_algoritmo = argv[1];
    const char *arquivo = argv[2];

    if(strcmp(nome_algoritmo, "rate") == 0){

    }else if(strcmp(nome_algoritmo, "edf") == 0){

    }else{
        fprintf(stderr, "Algoritmo invalido, use rate ou edf");
        exit(1);
    }

    ler_arquivo_entrada(arquivo);

    return 0;
}