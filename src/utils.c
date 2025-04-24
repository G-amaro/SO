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

    switch (argc) {
        case 6:
            if (strcmp(argv[1], "-a") == 0) {
                // operação -a title authors year path
                snprintf(mensagem, 512, "%s|%s|%s|%s|%s", argv[1], argv[2], argv[3], argv[4], argv[5]);
            } else {
                free(mensagem);
                return NULL;
            }
            break;

        case 3:
            if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "-s")==0) {

                snprintf(mensagem, 512, "%s|%s", argv[1], argv[2]);
            } else {
                free(mensagem);
                return NULL;
            }
            break;

        case 4:
            if (strcmp(argv[1], "-l") == 0) {
                // operação -l com key e palavra-chave
                snprintf(mensagem, 512, "%s|%s|%s", argv[1], argv[2], argv[3]);
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

    write(fd_resp, resposta, strlen(resposta) + 1);
    close(fd_resp);
}


int search_in_file(char* path, char* keyword) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro");
        return -1;
    }

    char buffer[1024];
    char linha[2048] = {0};
    ssize_t bytesRead;
    int count = 0;
    size_t linha_pos = 0;
    size_t keyword_len = strlen(keyword);

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n' || linha_pos >= sizeof(linha) - 1) {
                linha[linha_pos] = '\0';

                if (strstr(linha, keyword)) {
                    count++;
                }

                linha_pos = 0;
                linha[0] = '\0';
            } else {
                linha[linha_pos++] = buffer[i];
            }
        }
    }

    if (linha_pos > 0) {  // processar última linha se não terminar com '\n'
        linha[linha_pos] = '\0';
        if (strstr(linha, keyword)) {
            count++;
        }
    }

    close(fd);
    return count;
}


int search_in_file_once(char* path, char* keyword) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro");
        return -1;
    }

    char buffer[1024];
    char linha[2048];
    ssize_t bytesRead;
    size_t linha_pos = 0;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            // Se encontrar nova linha ou o buffer de linha estiver cheio
            if (buffer[i] == '\n' || linha_pos >= sizeof(linha) - 1) {
                linha[linha_pos] = '\0';  // Termina a string da linha

                // Verifica se a palavra-chave está presente na linha
                if (strstr(linha, keyword)) {
                    close(fd);
                    return 1;  // Palavra-chave encontrada
                }

                linha_pos = 0;  // Reseta a posição para a próxima linha
            } else {
                linha[linha_pos++] = buffer[i];  // Adiciona o caractere à linha
            }
        }
    }

    // Processa a última linha se não terminar com '\n'
    if (linha_pos > 0) {
        linha[linha_pos] = '\0';
        if (strstr(linha, keyword)) {
            close(fd);
            return 1;  // Palavra-chave encontrada
        }
    }

    close(fd);
    return 0;  // Palavra-chave não encontrada
}