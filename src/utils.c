#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"



void send_response_to_client(const char* resposta) {
    int fd_resp = open(FIFO_RESP_PATH, O_WRONLY);
    if (fd_resp == -1) {
        perror("Erro ao abrir o FIFO de resposta");
        return;
    }

    write(fd_resp, resposta, strlen(resposta) + 1);
    close(fd_resp);
}
