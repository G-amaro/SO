#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"



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

    // Ler a resposta do servidor (primeiro o tamanho, depois a mensagem)
    int fd_resp = open(FIFO_RESP_PATH, O_RDONLY);
    if (fd_resp == -1) {
        perror("Erro ao abrir o FIFO de resposta");
        return 1;
    }

    // Ler o tamanho da resposta
    int tamanho_resposta;
    if (read(fd_resp, &tamanho_resposta, sizeof(int)) == -1) {
        perror("Erro ao ler o tamanho da resposta");
        close(fd_resp);
        return 1;
    }

    // Ler a mensagem de acordo com o tamanho informado
    char* resposta = malloc(tamanho_resposta);
    if (resposta == NULL) {
        perror("Erro ao alocar memória para a resposta");
        close(fd_resp);
        return 1;
    }

    ssize_t total_bytes_lidos = 0;
    ssize_t bytes_lidos;

    // Continuar lendo até que todos os dados tenham sido recebidos
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

    resposta[tamanho_resposta-1] = '\0'; // Garantir que a resposta termina com '\0'
    printf("Resposta do servidor: %s\n", resposta);

    free(resposta);
    close(fd_resp);
    return 0;
}
