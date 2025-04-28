#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include <sys/wait.h>




// Função para criar a mensagem a ser enviada ao servidor
char* make_mensagem(int argc, char *argv[]) {
    char *mensagem = (char*)malloc(512 * sizeof(char));
    if (mensagem == NULL) {
        perror("Erro ao alocar memória para mensagem");
        return NULL;
    }

    switch (argc) {
        case 2:
            if (strcmp(argv[1], "-f") == 0){
                snprintf(mensagem, 512, "%s", argv[1]);
            }
            break;
        case 3:
            if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "-d") == 0) {

                snprintf(mensagem, 512, "%s|%s", argv[1], argv[2]);
            } else if(strcmp(argv[1], "-s")==0){
                snprintf(mensagem, 512, "%s|%s|%d", argv[1], argv[2], 0);
            }
            else {
                free(mensagem);
                return NULL;
            }
            break;

        case 4:
            if (strcmp(argv[1], "-l") == 0) {
                // operação -l com key e palavra-chave
                snprintf(mensagem, 512, "%s|%s|%s", argv[1], argv[2], argv[3]);
            } else if(strcmp(argv[1], "-s")==0){
                snprintf(mensagem, 512, "%s|%s|%d|%s", argv[1], argv[2], 1, argv[3]);
            }else {
                free(mensagem);
                return NULL;
            }
            break;
        case 6:
            if (strcmp(argv[1], "-a") == 0) {
                // operação -a title authors year path
                snprintf(mensagem, 512, "%s|%s|%s|%s|%s", argv[1], argv[2], argv[3], argv[4], argv[5]);
            } else {
                free(mensagem);
                return NULL;
            }
            break;

        default:
            free(mensagem);
            return NULL;
    }

    return mensagem;
}


void send_response_to_client(const char* resposta) {
    int fd_resp = open(FIFO_RESP_PATH, O_WRONLY);
    if (fd_resp == -1) {
        perror("Erro ao abrir o FIFO de resposta");
        return;
    }

    // Enviar o tamanho da mensagem
    int tamanho = strlen(resposta) + 1;
    if (write(fd_resp, &tamanho, sizeof(int)) == -1) {
        perror("Erro ao enviar tamanho da resposta");
        close(fd_resp);
        return;
    }

    // Enviar a mensagem real
    if (write(fd_resp, resposta, tamanho) == -1) {
        perror("Erro ao enviar mensagem");
        close(fd_resp);
        return;
    }

    close(fd_resp);
}
// Devolve número de linhas que contêm a keyword ou -1 em caso de erro
int search_in_file(const char* path, const char* keyword) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Erro ao criar pipe");
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Erro ao fazer fork");
        return -1;
    }

    if (pid == 0) {
        // Processo filho
        close(pipefd[0]); // Fecha leitura

        // Redireciona stdout para o pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Executa grep -c keyword path
        execlp("grep", "grep", "-c", keyword, path, NULL);

        // Se execlp falhar
        perror("Erro no execlp");
        exit(1);
    } else {
        // Processo pai
        close(pipefd[1]); // Fecha escrita

        char buffer[32] = {0};
        read(pipefd[0], buffer, sizeof(buffer));
        close(pipefd[0]);

        wait(NULL); // Espera que o filho termine

        return atoi(buffer);  // Converte e devolve o número de linhas
    }
}

int search_in_file_once(char* path, char* keyword) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Erro ao criar pipe");
        return 0;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Erro ao fazer fork");
        return 0;
    }

    if (pid == 0) {
        // Processo filho
        close(pipefd[0]); // Fecha leitura do pipe

        // Redireciona stdout para o pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Executa o comando grep -q para verificar se o arquivo contém a palavra-chave
        execlp("grep", "grep", "-q", keyword, path, (char*)NULL);

        // Se execlp falhar
        perror("Erro ao executar execlp");
        exit(1);
    } else {
        // Processo pai
        close(pipefd[1]); // Fecha escrita do pipe

        int status;
        waitpid(pid, &status, 0);  // Espera o processo filho terminar

        // Verifica o status do filho
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            close(pipefd[0]);
            return 1;  // Palavra-chave encontrada
        }

        close(pipefd[0]);
        return 0;  // Palavra-chave não encontrada
    }
}
