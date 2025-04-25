#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "operações/pesquisar.h"
#include "lista_ligada.h"
#include "utils.h"
void operacao_pesquisar(char* mensagem, char* folder) {
    char* keyword = strtok(mensagem + 3, "|");
    char* flag_n_proc = strtok(NULL, "|");

    if (!keyword || !flag_n_proc) {
        send_response_to_client("Erro: argumentos inválidos para -s");
        return;
    }

    int usar_processos = atoi(flag_n_proc);
    int max_processes = 1;

    if (usar_processos == 1) {
        char* n_processos_str = strtok(NULL, "|");
        if (!n_processos_str) {
            send_response_to_client("Erro: número de processos não especificado.");
            return;
        }
        max_processes = atoi(n_processos_str);
        if (max_processes <= 0) max_processes = 1;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Erro ao criar pipe");
        return;
    }

    Document* current = document_list;
    int current_processes = 0;

    while (current != NULL) {
        if (usar_processos == 1) {
            // Modo concorrente
            if (current_processes >= max_processes) {
                int status;
                wait(&status);
                current_processes--;
            }

            pid_t pid = fork();
            if (pid == 0) {
                // Processo filho
                close(pipefd[0]);
                char full_path[512];
                snprintf(full_path, sizeof(full_path), "%s/%s", folder, current->path);

                int found = search_in_file_once(full_path, keyword);
                if (found) {
                    write(pipefd[1], &current->id, sizeof(int));
                }
                exit(0);
            } else if (pid > 0) {
                current_processes++;
            } else {
                perror("Erro no fork");
            }

        } else {
            // Modo sequencial
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", folder, current->path);

            int found = search_in_file_once(full_path, keyword);
            if (found) {
                write(pipefd[1], &current->id, sizeof(int));
            }
        }

        current = current->next;
    }

    while (current_processes > 0) {
        int status;
        wait(&status);
        current_processes--;
    }

    close(pipefd[1]);

    // Lê os IDs encontrados
    int id;
    char resposta[1024] = "";
    int first = 1;

    while (read(pipefd[0], &id, sizeof(int)) > 0) {
        char temp[32];
        snprintf(temp, sizeof(temp), "%d", id);
        if (first) {
            strcat(resposta, temp);
            first = 0;
        } else {
            strcat(resposta, ", ");
            strcat(resposta, temp);
        }
    }

    close(pipefd[0]);

    if (strlen(resposta) == 0) {
        send_response_to_client("Nenhum documento contém a palavra.");
    } else {
        char mensagem_final[1100];
        snprintf(mensagem_final, sizeof(mensagem_final), "[%s]", resposta);
        send_response_to_client(mensagem_final);
    }

    free_documents();
    exit(0);
}
