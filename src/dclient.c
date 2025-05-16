#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"



int main(int argc, char *argv[]) {
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o FIFO");
        return 1;
    }

    char* mensagem = make_mensagem(argc, argv);
    if (mensagem == NULL) {
        fprintf(stderr, "Erro ao criar mensagem.\n");
        close(fd);
        return 1;
    }

    if (write(fd, mensagem, strlen(mensagem) + 1) == -1) {
        perror("Erro ao escrever no FIFO");
        free(mensagem);
        close(fd);
        return 1;
    }


    free(mensagem);
    close(fd);

    int fd_resp = open(FIFO_RESP_PATH, O_RDONLY);
    if (fd_resp == -1) {
        perror("Erro ao abrir o FIFO de resposta");
        return 1;
    }

    int tamanho_resposta;
    if (read(fd_resp, &tamanho_resposta, sizeof(int)) == -1) {
        perror("Erro ao ler o tamanho da resposta");
        close(fd_resp);
        return 1;
    }

    char* resposta = malloc(tamanho_resposta);
    if (resposta == NULL) {
        perror("Erro ao alocar memória para a resposta");
        close(fd_resp);
        return 1;
    }

    ssize_t total_bytes_lidos = 0;
    ssize_t bytes_lidos;


    while (total_bytes_lidos < tamanho_resposta) {
        bytes_lidos = read(fd_resp, resposta + total_bytes_lidos, tamanho_resposta - total_bytes_lidos);
        if (bytes_lidos == -1) {
            perror("Erro ao ler a resposta");
            free(resposta);
            close(fd_resp);
            return 1;
        }
        total_bytes_lidos += bytes_lidos;
    }

    resposta[tamanho_resposta-1] = '\0'; 
    printf("%s\n", resposta);

    free(resposta);
    close(fd_resp);
    return 0;
}
