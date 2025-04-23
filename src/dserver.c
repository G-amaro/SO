#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "manipulacao_listaligada.h"

#define FIFO_PATH "/tmp/dserver_fifo"
#define FIFO_RESP_PATH "/tmp/dclient_fifo"

void send_response_to_client(const char* resposta) {
    // Abrindo o FIFO de resposta
    int fd_resp = open(FIFO_RESP_PATH, O_WRONLY);
    if (fd_resp == -1) {
        perror("Erro ao abrir o FIFO de resposta");
        return;
    }

    // Enviando a resposta para o cliente
    write(fd_resp, resposta, strlen(resposta) + 1);  // +1 para incluir o '\0' no final
    close(fd_resp);
}

int main() {
    // Criando os FIFOs
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

    // Abrindo o FIFO para leitura
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

        // Processando a operação
        if (strncmp(mensagem, "-a", 2) == 0) {
            // Operação de indexação
            char* title = strtok(mensagem + 3, "|");
            char* authors = strtok(NULL, "|");
            char* year = strtok(NULL, "|");
            char* path = strtok(NULL, "|");

            if (title && authors && year && path) {

            
                
                send_response_to_client(add_document(title, authors, year, path));

            }
        } else if (strncmp(mensagem ,"-c", 2) == 0) {
            char* key = strtok(mensagem + 3, "|");

            if(key) {
                printf("Operação '-c' consultar: %s\n", key);
                //funcao
            
                char resposta[100];
                snprintf(resposta, sizeof(resposta), "Documento com ID=%s consultado", key);
            
                send_response_to_client(resposta);
            }

        } else if (strncmp(mensagem ,"-d", 2) == 0) {
            char* key = strtok(mensagem + 3, "|");

            if(key) {
                printf("Operação '-d' eliminar: %s\n", key);
                
                send_response_to_client(delete_document(key));
            }
        }else {
            printf("Operação desconhecida: %s\n", mensagem);
        }
    }

    close(fd);
    free_documents();  // Liberar a memória dos documentos
    return 0;
}
