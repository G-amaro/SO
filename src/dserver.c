#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "utils.h"
#include "lista_ligada.h"
#include "ficheiro_presistencia.h"
#include "operações/adicionar.h"
#include "operações/consultar.h"
#include "operações/eliminar.h"
#include "operações/contar.h"
#include "operações/pesquisar.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <diretório de documentos>\n", argv[0]);
        return 1;
    }

    // Inicialização
    // clear_persistence_file();
    init_persistence_file();
    list_documents_in_persistence();
    load_documents();
    list_documents();

    // Criação dos FIFOs
    if (access(FIFO_PATH, F_OK) == -1 && mkfifo(FIFO_PATH, 0666) == -1) {
        perror("Erro ao criar o FIFO");
        return 1;
    }
    if (access(FIFO_RESP_PATH, F_OK) == -1 && mkfifo(FIFO_RESP_PATH, 0666) == -1) {
        perror("Erro ao criar o FIFO de resposta");
        return 1;
    }

    printf("FIFOs prontos!\n");

    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o FIFO");
        return 1;
    }

    while (1) {
        char mensagem[512];
        ssize_t n = read(fd, mensagem, sizeof(mensagem));
        if (n <= 0) continue;

        mensagem[n] = '\0';
        char operacao = mensagem[1];

        switch (operacao) {
            case 'a':
                operacao_adicionar(mensagem);
                break;

            case 'd':
                operacao_eliminar(mensagem);
                break;

            case 'c': {
                pid_t pid = fork();
                if (pid == 0) {
                    operacao_consultar(mensagem);
                } else if (pid < 0) {
                    perror("Erro no fork");
                }
                break;
            }

            case 'l': {
                pid_t pid = fork();
                if (pid == 0) {
                    operacao_contar(mensagem, argv[1]);
                } else if (pid < 0) {
                    perror("Erro no fork");
                }
                break;
            }

            case 's': {
                pid_t pid = fork();
                if (pid == 0) {
                    operacao_pesquisar(mensagem, argv[1]);
                } else if (pid < 0) {
                    perror("Erro no fork");
                }
                break;
            }

            default:
                printf("Operação desconhecida: %s\n", mensagem);
                break;
        }

        list_documents_in_persistence();
        list_documents();
    }

    close(fd);
    free_documents();
    save_documents_to_persistence();
    return 0;
}
