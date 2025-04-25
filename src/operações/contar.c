#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "operações/contar.h"
#include "lista_ligada.h"
#include "utils.h"

void operacao_contar(char* mensagem, char* folder) {
    char* id = strtok(mensagem + 3, "|");
    char* keyword = strtok(NULL, "|");
    char* filename = get_path_by_id(atoi(id));

    if (!filename) {
        printf("Arquivo não encontrado\n");
        exit(1);
    }

    char path[strlen(folder) + strlen(filename) + 3];  
    snprintf(path, sizeof(path), "%s/%s", folder, filename);
    printf("path: %s\n", path);

    int num_linhas = search_in_file(path, keyword);

    if (num_linhas >= 0) {
        char resposta[256];
        snprintf(resposta, sizeof(resposta), "%d", num_linhas);
        send_response_to_client(resposta);
    }

    free_documents();
    exit(0);
}
