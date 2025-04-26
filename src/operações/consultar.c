#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "operações/consultar.h"    
#include "lista_ligada.h"
#include "utils.h"
#include "cache.h"

void operacao_consultar(char* key) {
    if (key) {
        printf("Operação '-c' consultar: %s\n", key);

        char* resposta = (char*)malloc(512 * sizeof(char));;
        Document* doc_cache = find_in_cache(atoi(key));
        
        if (doc_cache != NULL) {

            snprintf(resposta, 512, "Title: %s;\nAuthors: %s;\nYear: %s;\nPath: %s;\n", doc_cache->title, doc_cache->authors, doc_cache->year, doc_cache->path);
        } else {
            doc_cache=get_doc_by_id(atoi(key));
            if(doc_cache!=NULL) {
                add_to_cache(doc_cache);

                snprintf(resposta, 512, "Title: %s;\nAuthors: %s;\nYear: %s;\nPath: %s;\n", doc_cache->title, doc_cache->authors, doc_cache->year, doc_cache->path);
            }else{

                snprintf(resposta, 512, "Documento com ID=%s não encontrado", key);

            }

        }

        printf("%s\n", resposta); 
        send_response_to_client(resposta);
        free(resposta);

    }

    
}
