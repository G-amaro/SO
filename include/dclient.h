#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Definição dos caminhos para os FIFOs
#define FIFO_PATH "/tmp/fifo_cliente"  // Caminho para o FIFO de escrita
#define FIFO_RESP_PATH "/tmp/fifo_resposta"  // Caminho para o FIFO de resposta

// Protótipos das funções
char* make_mensagem(int argc, char *argv[]);

#endif // CLIENT_H
