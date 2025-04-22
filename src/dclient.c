#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FIFO_PATH "/tmp/dserver_fifo"
#define FIFO_RESP_PATH "/tmp/dclient_fifo"

char* make_mensagem(char* argv[]) {
    char *operacao = argv[1];
    char *mensagem = NULL;

    if (strcmp(operacao, "-a") == 0) {
        // Verificar se os parâmetros necessários estão presentes
        if (argv[2] == NULL || argv[3] == NULL || argv[4] == NULL) {
            fprintf(stderr, "Erro: operação -a requer título, autores e ano.\n");
            return NULL;
        }

        // Obter os parâmetros
        char *title = argv[2];
        char *authors = argv[3];
        char *year = argv[4];
        char *path = (argv[5] != NULL) ? argv[5] : ""; // Se não for passado, usa string vazia.

        // Calcular o tamanho necessário para a string
        int tamanho_mensagem = strlen(operacao) + strlen(title) + strlen(authors) + strlen(year) + strlen(path) + 5; // 5 para os separadores '|' e o '\0'

        // Alocar memória para a mensagem
        mensagem = (char*)malloc(tamanho_mensagem * sizeof(char));
        if (mensagem == NULL) {
            perror("Erro ao alocar memória");
            return NULL;
        }

        // Criar a mensagem formatada
        snprintf(mensagem, tamanho_mensagem, "%s|%s|%s|%s|%s", operacao, title, authors, year, path);

    } else if ((strcmp(operacao, "-c") == 0) || (strcmp(operacao, "-d") == 0)) {
        // Operações -c e -d
        if (argv[2] == NULL) {
            fprintf(stderr, "Erro: operação -c ou -d requer chave.\n");
            return NULL;
        }
        char *key = argv[2];

        // Calcular o tamanho necessário para a string
        mensagem = (char*)malloc(strlen(operacao) + strlen(key) + 2); // +2 para o separador '|' e o '\0'
        if (mensagem == NULL) {
            perror("Erro ao alocar memória");
            return NULL;
        }

        // Criar a mensagem formatada
        snprintf(mensagem, strlen(operacao) + strlen(key) + 2, "%s|%s", operacao, key);

    } else if (strcmp(operacao, "-l") == 0) {
        // Operação -l
        if (argv[2] == NULL || argv[3] == NULL) {
            fprintf(stderr, "Erro: operação -l requer chave e palavra-chave.\n");
            return NULL;
        }
        char *key = argv[2];
        char *keyword = argv[3];

        // Calcular o tamanho necessário para a string
        mensagem = (char*)malloc(strlen(operacao) + strlen(key) + strlen(keyword) + 3); // +3 para os separadores '|' e o '\0'
        if (mensagem == NULL) {
            perror("Erro ao alocar memória");
            return NULL;
        }

        // Criar a mensagem formatada
        snprintf(mensagem, strlen(operacao) + strlen(key) + strlen(keyword) + 3, "%s|%s|%s", operacao, key, keyword);

    } else if (strcmp(operacao, "-s") == 0) {
        // Operação -s
        if (argv[2] == NULL) {
            fprintf(stderr, "Erro: operação -s requer palavra-chave.\n");
            return NULL;
        }
        char *keyword = argv[2];

        // Calcular o tamanho necessário para a string
        mensagem = (char*)malloc(strlen(operacao) + strlen(keyword) + 2); // +2 para o separador '|' e o '\0'
        if (mensagem == NULL) {
            perror("Erro ao alocar memória");
            return NULL;
        }

        // Criar a mensagem formatada
        snprintf(mensagem, strlen(operacao) + strlen(keyword) + 2, "%s|%s", operacao, keyword);

    } else if (strcmp(operacao, "-f") == 0) {
        // Operação -f (sem parâmetros adicionais no momento)
        // Podemos apenas retornar uma mensagem simples
        mensagem = (char*)malloc(strlen(operacao) + 1); // Só o nome da operação e o '\0'
        if (mensagem == NULL) {
            perror("Erro ao alocar memória");
            return NULL;
        }

        snprintf(mensagem, strlen(operacao) + 1, "%s", operacao);

    } else {
        // Operação inválida
        return NULL;
    }

    return mensagem;
}

int main(int argc, char *argv[]) {
    // Verificar o número de argumentos
    if (argc < 3) {
        fprintf(stderr, "Erro: operação e argumentos insuficientes.\n");
        return 1;
    }

    // Abrir o FIFO para escrita
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o FIFO");
        return 1;
    }

    // Gerar a mensagem a ser enviada
    char* mensagem = make_mensagem(argv);
    if (mensagem == NULL) {
        fprintf(stderr, "Erro ao criar mensagem.\n");
        close(fd);
        return 1;
    }

    // Enviar a mensagem através do FIFO
    if (write(fd, mensagem, strlen(mensagem) + 1) == -1) {
        perror("Erro ao escrever no FIFO");
        free(mensagem);
        close(fd);
        return 1;
    }

    printf("Pedido enviado para o servidor.\n");

    // Liberar a memória da mensagem
    free(mensagem);

    close(fd);

    // Ler a resposta do servidor
    int fd_resp = open(FIFO_RESP_PATH, O_RDONLY);
    if (fd_resp == -1) {
        perror("Erro ao abrir o FIFO de resposta");
        return 1;
    }

    char resposta[256];
    ssize_t n = read(fd_resp, resposta, sizeof(resposta));
    if (n == -1) {
        perror("Erro ao ler a resposta do FIFO");
        close(fd_resp);
        return 1;
    }

    resposta[n] = '\0'; // Garantir que a resposta termina com '\0'
    printf("Resposta do servidor: %s\n", resposta);

    close(fd_resp);
    return 0;
}
