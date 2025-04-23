#ifndef UTILS_H
#define UTILS_H

#define FIFO_PATH "/tmp/dserver_fifo"
#define FIFO_RESP_PATH "/tmp/dclient_fifo"

// Envia uma resposta do servidor para o cliente via FIFO
void send_response_to_client(const char* resposta);

#endif // UTILS_H
