#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define FIFO_PATH "/tmp/dserver_fifo"
#define FIFO_RESP_PATH "/tmp/dclient_fifo"

int main() {
    // Abrir o FIFO para escrita
    // int fd = open(FIFO_PATH, O_WRONLY);
    // if (fd == -1) {
    //     perror("Erro ao abrir o FIFO");
    //     return 1;
    // }
    //
    // // Enviar um pedido ao servidor
    // char *mensagem = "Comando para o servidor";
    // write(fd, mensagem, strlen(mensagem) + 1); // +1 para incluir o '\0'
    //
    // printf("Pedido enviado para o servidor.\n");
    //
    // close(fd);
    //
    // // Ler a resposta do servidor
    // int fd_resp = open(FIFO_RESP_PATH, O_RDONLY);
    // if (fd_resp == -1) {
    //     perror("Erro ao abrir o FIFO de resposta");
    //     return 1;
    // }
    //
    // char resposta[256];
    // read(fd_resp, resposta, sizeof(resposta));
    // printf("Resposta do servidor: %s\n", resposta);
    //
    // close(fd_resp);
    // return 0;
}
