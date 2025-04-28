#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "utils.h"
#include "lista_ligada.h"
#include "ficheiro_presistencia.h"
#include "operações/adicionar.h"
#include "operações/consultar.h"
#include "operações/eliminar.h"
#include "operações/contar.h"
#include "operações/pesquisar.h"
#include "cache.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <diretório de documentos> <tamanho da cache>\n", argv[0]);
        return 1;
    }
    char* folder = argv[1];
    int size_cache = atoi(argv[2]); 


    // Inicialização
    // clear_persistence_file();
    init_persistence_file();
    //list_documents_in_persistence();
    load_documents();
    //list_documents();
    init_cache(size_cache);

    // Criação dos FIFOs
    if (access(FIFO_PATH, F_OK) == -1 && mkfifo(FIFO_PATH, 0666) == -1) {
        perror("Erro ao criar o FIFO");
        return 1;
    }
    if (access(FIFO_RESP_PATH, F_OK) == -1 && mkfifo(FIFO_RESP_PATH, 0666) == -1) {
        perror("Erro ao criar o FIFO de resposta");
        return 1;
    }

    printf("FIFOs prontos!\n");

    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o FIFO");
        return 1;
    }

    while (1) {
        char mensagem[512];
        ssize_t n = read(fd, mensagem, sizeof(mensagem));
        if (n <= 0) continue;

        mensagem[n] = '\0';
        char operacao = mensagem[1];

        switch (operacao) {
            case 'a':
                operacao_adicionar(mensagem);
                break;

            case 'd':
                operacao_eliminar(mensagem);
                break;

            case 'c': {
                char* key = strtok(mensagem + 3, "|");
                pid_t pid = fork();
                if (pid == 0) {
                    operacao_consultar(key);
                    exit(0);
                } else if (pid < 0) {
                    perror("Erro no fork");
                }
                Document* doc =get_doc_by_id(atoi(key));
                if(doc!=NULL) add_to_cache(doc);
                break;
            }

            case 'l': {
                char* id = strtok(mensagem + 3, "|");
                char* keyword = strtok(NULL, "|");
                Document * doc_cache = find_in_cache(atoi(id));
                char* filename;
                if(doc_cache!=NULL){

                    filename = doc_cache->path;
                }else{
                    Document* doc = get_doc_by_id(atoi(id));
                    if (doc != NULL) {
                        
                        filename = doc->path;
                        add_to_cache(doc);
                    }
                }
                pid_t pid = fork();
                if (pid == 0) {
                    operacao_contar(filename, folder, keyword);
                    exit(0);
                } else if (pid < 0) {
                    perror("Erro no fork");
                }
                break;
            }

            case 's': {
                pid_t pid = fork();
                if (pid == 0) {
                    operacao_pesquisar(mensagem, folder);
                    exit(0);
                } else if (pid < 0) {
                    perror("Erro no fork");
                }
                break;
            }
            case 'f': {
                printf("Comando de término recebido. Encerrando servidor...\n");
                send_response_to_client("Server is shuting down");
                save_documents_to_persistence();  
                free_cache(); 
                free_documents();                  
                
                close(fd);  
                unlink(FIFO_PATH); 
                unlink(FIFO_RESP_PATH);
            
                exit(0);
            }
            default:
                printf("Operação desconhecida: %s\n", mensagem);
                break;
        }

        //list_documents_in_persistence();
        //list_documents_cache();
        //list_documents();
    }

    close(fd);
    free_documents();
    save_documents_to_persistence();
    return 0;
}
