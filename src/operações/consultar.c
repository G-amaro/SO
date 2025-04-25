#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "operações/consultar.h"    
#include "lista_ligada.h"
#include "utils.h"

void operacao_consultar(char* mensagem) {
    char* key = strtok(mensagem + 3, "|");
    if (key) {
        printf("Operação '-c' consultar: %s\n", key);
        char* resposta = search_document(key);
        send_response_to_client(resposta);
    }
    free_documents();
    exit(0);
}
