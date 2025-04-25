#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "lista_ligada.h"
#include "ficheiro_presistencia.h"

int main(int argc, char *argv[]) {
    // Inicializar o arquivo de persistência
    clear_persistence_file();
    init_persistence_file();
    list_documents_in_persistence();
    load_documents();
    list_documents();

    // Criar os FIFOs se necessário
    if (access(FIFO_PATH, F_OK) == -1) {
        if (mkfifo(FIFO_PATH, 0666) == -1) {
            perror("Erro ao criar o FIFO");
            return 1;
        }
        printf("FIFO de requisição criado com sucesso!\n");
    } else {
        printf("FIFO de requisição já existe.\n");
    }

    if (access(FIFO_RESP_PATH, F_OK) == -1) {
        if (mkfifo(FIFO_RESP_PATH, 0666) == -1) {
            perror("Erro ao criar o FIFO de resposta");
            return 1;
        }
        printf("FIFO de resposta criado com sucesso!\n");
    } else {
        printf("FIFO de resposta já existe.\n");
    }

    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o FIFO");
        return 1;
    }

    // Loop principal do servidor
    while (1) {
        char mensagem[512];
        ssize_t n = read(fd, mensagem, sizeof(mensagem));
        if (n <= 0) {
            continue;
        }

        mensagem[n] = '\0';

        // A operação é dada por mensagem[1]
        char operacao = mensagem[1];

        switch (operacao) {
            case 'a': {
                // Operação de adicionar documento (sem fork)
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
                
                break;
            }

            case 'd': {
                // Operação de eliminar documento (sem fork)
                char* key = strtok(mensagem + 3, "|");
                if (key) {
                    printf("Operação '-d' eliminar: %s\n", key);
                    char* resposta = delete_document(key);
                    remove_from_persistence_file(atoi(key));
                    send_response_to_client(resposta);
                }
                
                break;
            }

            case 'c': {
                // Operação de consultar documento - fork
                pid_t pid = fork();
                if (pid == 0) {
                    // Processo filho — trata a operação de consulta
                    char* key = strtok(mensagem + 3, "|");
                    if (key) {
                        printf("Operação '-c' consultar: %s\n", key);
                        char* resposta = search_document(key);
                        send_response_to_client(resposta);
                    }
                    free_documents();  // Liberta a memória alocada no filho
                    exit(0);  // Termina o processo filho
                } else if (pid < 0) {
                    perror("Erro no fork");
                }
                break;
            }

            case 'l': {
                // Operação de listar documentos - fork
                pid_t pid = fork();
                if (pid == 0) {
                    // Processo filho — trata a operação de listar documentos
                    char* id = strtok(mensagem + 3, "|");
                    char* keyword = strtok(NULL, "|");
                    char* folder = argv[1];
                    char* filename = get_path_by_id(atoi(id));

                    if (!filename) {
                        printf("Arquivo não encontrado\n");
                        break;
                    }

                    char path[strlen(folder) + strlen(filename) + 3];  
                    snprintf(path, sizeof(path), "%s/%s", folder, filename);
                    printf("path: %s\n", path);

                    int num_linhas = search_in_file(path, keyword);

                    if (num_linhas >= 0) {
                        char resposta[256];
                        snprintf(resposta, sizeof(resposta), "Número de linhas contendo '%s': %d", keyword, num_linhas);
                        send_response_to_client(resposta);
                    }
                    free_documents();  // Liberta a memória alocada no filho
                    exit(0);  // Termina o processo filho
                } else if (pid < 0) {
                    perror("Erro no fork");
                }
                break;
            }

            case 's': {
                // Operação de pesquisa - fork
                pid_t pid = fork();
                if (pid == 0) {
                    // Processo filho — trata a operação de pesquisa
                    int counter = 0;
                    char* folder = argv[1]; 
                    Document* current = document_list;
                    char* keyword = strtok(mensagem + 3, "|");

                    while (current != NULL) {
                        char* filename = current->path;
                        char path[strlen(folder) + strlen(filename) + 3];  
                        snprintf(path, sizeof(path), "%s/%s", folder, filename);

                        int num_linhas = search_in_file_once(path, keyword);
                        if (num_linhas) {
                            counter++;
                        }
                        current = current->next;
                    }

                    char resposta[256];
                    snprintf(resposta, sizeof(resposta), "Número de documentos contendo '%s': %d", keyword, counter);
                    send_response_to_client(resposta);

                    free_documents();  // Liberta a memória alocada no filho
                    exit(0);  // Termina o processo filho
                } else if (pid < 0) {
                    perror("Erro no fork");
                }
                break;
            }

            default:
                printf("Operação desconhecida: %s\n", mensagem);
                break;
        }
        list_documents_in_persistence();
                list_documents();
    }

    close(fd);
    free_documents();
    save_documents_to_persistence();
    return 0;
}
