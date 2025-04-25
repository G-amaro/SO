#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operações/adicionar.h"
#include "lista_ligada.h"
#include "ficheiro_presistencia.h"
#include "utils.h"

void operacao_adicionar(char* mensagem) {
    char* title = strtok(mensagem + 3, "|");
    char* authors = strtok(NULL, "|");
    char* year = strtok(NULL, "|");
    char* path = strtok(NULL, "|");
    int exists = 0;

    if (title && authors && year && path) {
        int next_id = get_first_document_id() + 1;
        printf("next id=%d\n", next_id);
        char* resposta = add_document(title, authors, year, path, next_id, &exists);
        Document* new_doc = document_list;
        add_to_persistence_file(new_doc, &exists);
        send_response_to_client(resposta);
    }
}
