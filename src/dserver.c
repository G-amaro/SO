#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define FIFO_PATH "/tmp/dserver_fifo"
#define FIFO_RESP_PATH "/tmp/dclient_fifo"

int main() {
      //Verificar se o FIFO já existe antes de criar
      if (access(FIFO_PATH, F_OK) == -1) {   // Verifica se o arquivo existe
          if (mkfifo(FIFO_PATH, 0666) == -1) {
              perror("Erro ao criar o FIFO");
              return 1;
          }
          printf("Pipe nomeado criado com sucesso!\n");
      } else {
          printf("O FIFO já existe, prosseguindo...\n");
      }

        //Verificar se o FIFO de resposta já existe antes de criar
      if (access(FIFO_RESP_PATH, F_OK) == -1) {   // Verifica se o arquivo existe
          if (mkfifo(FIFO_RESP_PATH, 0666) == -1) {
              perror("Erro ao criar o FIFO de resposta");
              return 1;
          }
          printf("FIFO de resposta criado com sucesso!\n");
      } else {
          printf("O FIFO de resposta já existe, prosseguindo...\n");
      }

       // Abrir o FIFO para leitura
      int fd = open(FIFO_PATH, O_RDONLY);
      if (fd == -1) {
          perror("Erro ao abrir o FIFO");
          return 1;
      }

       // Ler o pedido do cliente
      char mensagem[256];
      read(fd, mensagem, sizeof(mensagem));
      printf("Pedido recebido do cliente: %s\n", mensagem);

       // Processar a requisição e enviar a resposta
      int fd_resp = open(FIFO_RESP_PATH, O_WRONLY);
      if (fd_resp == -1) {
          perror("Erro ao abrir o FIFO de resposta");
          return 1;
      }

      char *resposta = "Resultado da operação";
    write(fd_resp, resposta, strlen(resposta) + 1); // Enviar a resposta

    printf("Resposta enviada para o cliente.\n");

    close(fd);
    close(fd_resp);
    return 0;
}

