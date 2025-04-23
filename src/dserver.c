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

int next_id = 1;

int main() {
    // Inicializar o arquivo de persistência, carregando os documentos no servidor

    // init_persistence_file();
    // list_documents_in_persistence();
    // next_id = get_max_id_from_persistence() + 1;
    // load_documents_from_persistence();
    // list_documents();
    

    
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

        
        if (strncmp(mensagem, "-a", 2) == 0) {
            
            char* title = strtok(mensagem + 3, "|");
            char* authors = strtok(NULL, "|");
            char* year = strtok(NULL, "|");
            char* path = strtok(NULL, "|");

            if (title && authors && year && path) {
                char* resposta = add_document(title, authors, year, path, &next_id);

                // Adicionar o documento ao arquivo de persistência
                Document* new_doc = document_list;  // Novo documento adicionado à lista
                // add_to_persistence_file(new_doc);

                // Enviar a resposta ao cliente
                send_response_to_client(resposta);
            }
        } else if (strncmp(mensagem ,"-c", 2) == 0) {
            char* key = strtok(mensagem + 3, "|");

            if(key) {
                printf("Operação '-c' consultar: %s\n", key);
                
                char* resposta = search_document(key);
                
                // Enviar a resposta ao cliente
                send_response_to_client(resposta);
            }   

        } else if (strncmp(mensagem ,"-d", 2) == 0) {
            char* key = strtok(mensagem + 3, "|");

            if(key) {
                printf("Operação '-d' eliminar: %s\n", key);

                // Remover o documento da lista e do arquivo de persistência
                char* resposta = delete_document(key);
                // remove_from_persistence_file(atoi(key)); // Passa o ID do documento para remoção da persistência

                // Enviar a resposta ao cliente
                send_response_to_client(resposta);
            }
        } else {
            printf("Operação desconhecida: %s\n", mensagem);
        }
        // list_documents_in_persistence();
    }

    close(fd);
    free_documents();  // Liberar a memória dos documentos

    // Salvar os documentos restantes no arquivo de persistência antes de sair
    // save_documents_to_persistence();
    

    return 0;
}
