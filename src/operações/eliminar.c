#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operações/eliminar.h"
#include "lista_ligada.h"
#include "ficheiro_presistencia.h"
#include "utils.h"

void operacao_eliminar(char* mensagem) {
    char* key = strtok(mensagem + 3, "|");
    if (key) {
        printf("Operação '-d' eliminar: %s\n", key);
        char* resposta = delete_document(key);
        remove_from_persistence_file(atoi(key));
        send_response_to_client(resposta);
    }
}
