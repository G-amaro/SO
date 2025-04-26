#ifndef UTILS_H
#define UTILS_H

#define FIFO_PATH "/tmp/dserver_fifo"
#define FIFO_RESP_PATH "/tmp/dclient_fifo"

char* make_mensagem(int argc, char *argv[]);

void send_response_to_client(const char* resposta);
int search_in_file(const char* path, const char* keyword);
int search_in_file_once(char* path, char* keyword);
#endif // UTILS_H
