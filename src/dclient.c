#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"



// Função para criar a mensagem a ser enviada ao servidor
char* make_mensagem(int argc, char *argv[]) {
    char *mensagem = (char*)malloc(512 * sizeof(char));
    if (mensagem == NULL) {
        perror("Erro ao alocar memória para mensagem");
        return NULL;
    }
    if (argc == 6 && strcmp(argv[1],"-a") == 0) {

        snprintf(mensagem, 512, "%s|%s|%s|%s|%s", argv[1], argv[2], argv[3], argv[4], argv[5]);

    } else if (argc == 3){
        if(strcmp(argv[1],"-c") == 0 || strcmp(argv[1],"-d") == 0){
            //operação e key
            snprintf(mensagem, 512, "%s|%s", argv[1], argv[2]);
        }
    }



    return mensagem;
}

int main(int argc, char *argv[]) {

    // Abrir o FIFO para escrita
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o FIFO");
        return 1;
    }

    // Gerar a mensagem a ser enviada
    char* mensagem = make_mensagem(argc, argv);
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

    // Ler a resposta do servidor (identificador único)
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
    printf("%s\n", resposta);

    close(fd_resp);
    return 0;
}

