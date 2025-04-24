#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "lista_ligada.h"
#include "ficheiro_presistencia.h"  // Incluir o arquivo de persistência

int next_id = 0;



int main(int argc, char *argv[]) {
    
    // Inicializar o arquivo de persistência, carregando os documentos no servidor
    clear_persistence_file();
    init_persistence_file();
    list_documents_in_persistence();
    next_id = load_documents_and_get_max_id() + 1;
    list_documents();

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
        char mensagem[512];  // Buffer para ler a mensagem do cliente
        ssize_t n = read(fd, mensagem, sizeof(mensagem));
        if (n == -1) {
            perror("Erro ao ler do FIFO");
            continue;
        }

        if (n == 0) {
            continue;  // Nenhuma mensagem recebida, continuar no loop
        }

        mensagem[n] = '\0';  // Garantir que a string seja terminada corretamente

        char operacao = mensagem[1];  // Assumimos que as mensagens vêm com o formato "-x ..."

        switch (operacao) {
            case 'a': {
                char* title = strtok(mensagem + 3, "|");
                char* authors = strtok(NULL, "|");
                char* year = strtok(NULL, "|");
                char* path = strtok(NULL, "|");
                int exists = 0;

                if (title && authors && year && path) {
                    char* resposta = add_document(title, authors, year, path, &next_id, &exists);
                    Document* new_doc = document_list;  // Aponta para o novo documento
                    add_to_persistence_file(new_doc, &exists);
                    send_response_to_client(resposta);
                }
                list_documents_in_persistence();
                list_documents();
                break;
            }

            case 'c': {
                char* key = strtok(mensagem + 3, "|");
                if (key) {
                    printf("Operação '-c' consultar: %s\n", key);
                    char* resposta = search_document(key);
                    send_response_to_client(resposta);
                }
                break;
            }

            case 'd': {
                char* key = strtok(mensagem + 3, "|");
                if (key) {
                    printf("Operação '-d' eliminar: %s\n", key);
                    char* resposta = delete_document(key);
                    remove_from_persistence_file(atoi(key));
                    send_response_to_client(resposta);
                }
                list_documents_in_persistence();
                list_documents();
                break;
            }

            case 'l': {
                char* id = strtok(mensagem + 3, "|");
                char* keyword = strtok(NULL, "|");
                char* folder = argv[1];  // Diretório fornecido como argumento
                
                // Obter o caminho do ficheiro
                char* filename = get_path_by_id(atoi(id));
                
                if (!filename) {
                    printf("Arquivo não encontrado\n");
                    break;
                }
                
                // Construir o caminho completo para o arquivo
                char path[strlen(folder) + strlen(filename) + 3];  
                snprintf(path, sizeof(path), "%s/%s", folder, filename);
                printf("path: %s\n",path);
                // Procurar a palavra-chave no arquivo
                int num_linhas = search_in_file(path, keyword);
                
                if (num_linhas >= 0) {
                    char resposta[256];
                    snprintf(resposta, sizeof(resposta), "Número de linhas contendo '%s': %d", keyword, num_linhas);
                    send_response_to_client(resposta);
                }
                break;
            }
            case 's':
                int counter = 0;
                char* folder = argv[1]; 
                Document* current = document_list;
                char* keyword = strtok(mensagem + 3, "|");

                while (current != NULL) {
                    
                    char* folder = argv[1];  // Diretório fornecido como argumento
                    
                    // Obter o caminho do ficheiro
                    char* filename = current->path;
                    
                    // Construir o caminho completo para o arquivo
                    char path[strlen(folder) + strlen(filename) + 3];  
                    snprintf(path, sizeof(path), "%s/%s", folder, filename);

                    // Procurar a palavra-chave no arquivo
                    int num_linhas = search_in_file_once(path, keyword);
                    if(num_linhas){
                        counter++;
                    }
                    current = current->next;
                }
                char resposta[256];
                snprintf(resposta, sizeof(resposta), "Número de documentos contendo '%s': %d", keyword, counter);
                send_response_to_client(resposta);
                break;

                
            default:
                printf("Operação desconhecida: %s\n", mensagem);
                break;
        }
        
    }

    close(fd);
    free_documents();  // Liberar a memória dos documentos

    // Salvar os documentos restantes no arquivo de persistência antes de sair
    save_documents_to_persistence();
    
    return 0;
}