#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FIFO_PATH "/tmp/dserver_fifo"
#define FIFO_RESP_PATH "/tmp/dclient_fifo"

// Definindo a estrutura para armazenar informações de cada documento
typedef struct Document {
    int id;
    char title[201];
    char authors[201];
    char year[5];
    char path[65];
    struct Document* next;  // Ponteiro para o próximo documento
} Document;

// Lista ligada de documentos indexados
Document* document_list = NULL;
int next_id = 1;  // Contador de ID para documentos

// Função para adicionar um documento à lista ligada
void add_document(char* title, char* authors, char* year, char* path) {
    Document* new_doc = (Document*)malloc(sizeof(Document));
    if (!new_doc) {
        perror("Erro ao alocar memória para documento");
        return;
    }

    new_doc->id = next_id++;
    strncpy(new_doc->title, title, 200);
    strncpy(new_doc->authors, authors, 200);
    strncpy(new_doc->year, year, 4);
    strncpy(new_doc->path, path, 64);
    new_doc->next = document_list;
    document_list = new_doc;

    // Exibindo as informações no servidor para fins de depuração
    printf("Documento indexado: ID=%d, Title=%s, Authors=%s, Year=%s, Path=%s\n",
           new_doc->id, new_doc->title, new_doc->authors, new_doc->year, new_doc->path);
}

// Função para liberar a memória da lista de documentos
void free_documents() {
    Document* current = document_list;
    Document* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
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
                add_document(title, authors, year, path);

                // Enviar resposta para o cliente
                int fd_resp = open(FIFO_RESP_PATH, O_WRONLY);
                if (fd_resp == -1) {
                    perror("Erro ao abrir o FIFO de resposta");
                    continue;
                }

                char resposta[100];
                snprintf(resposta, sizeof(resposta), "Documento indexado com ID=%d", next_id - 1);
                write(fd_resp, resposta, strlen(resposta) + 1);
                close(fd_resp);
            }
        } else {
            printf("Operação desconhecida: %s\n", mensagem);
        }
    }

    close(fd);
    free_documents();  // Liberar a memória dos documentos
    return 0;
}
