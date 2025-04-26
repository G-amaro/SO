#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operações/eliminar.h"
#include "lista_ligada.h"
#include "ficheiro_presistencia.h"
#include "utils.h"
#include "cache.h"

void operacao_eliminar(char* mensagem) {
    char* key = strtok(mensagem + 3, "|");
    int id = atoi(key);
    int exist=0;
    if (key) {
        printf("Operação '-d' eliminar: %s\n", key);
        remove_from_cache(id);
        char* resposta = delete_document(key, &exist);

        if (exist == 1) {
            remove_from_persistence_file(id);
        }

        send_response_to_client(resposta);
        free(resposta); // não te esqueças de libertar a resposta!
    }
}
