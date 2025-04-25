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
    char* proc_str = strtok(NULL, "|");

    if (!keyword || !proc_str) {
        send_response_to_client("Erro: argumentos inválidos para -s");
        return;
    }

    int max_processes = atoi(proc_str);
    if (max_processes <= 0) max_processes = 1;

    int current_processes = 0;
    int total_found = 0;

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Erro ao criar pipe");
        return;
    }

    Document* current = document_list;

    while (current != NULL) {
        if (current_processes >= max_processes) {
            int status;
            wait(&status);
            current_processes--;
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(pipefd[0]);
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", folder, current->path);

            int found = search_in_file_once(full_path, keyword);
            write(pipefd[1], &found, sizeof(int));
            exit(0);
        } else if (pid > 0) {
            current_processes++;
        } else {
            perror("Erro no fork");
        }

        current = current->next;
    }

    while (current_processes > 0) {
        int status;
        wait(&status);
        current_processes--;
    }

    close(pipefd[1]);

    int found;
    while (read(pipefd[0], &found, sizeof(int)) > 0) {
        if (found) total_found++;
    }
    close(pipefd[0]);

    char resposta[256];
    snprintf(resposta, sizeof(resposta), "Número de documentos contendo '%s': %d", keyword, total_found);
    send_response_to_client(resposta);

    free_documents();
    exit(0);
}


