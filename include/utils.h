#ifndef UTILS_H
#define UTILS_H

#define FIFO_PATH "/tmp/dserver_fifo"
#define FIFO_RESP_PATH "/tmp/dclient_fifo"

/**
 * @brief Cria uma mensagem a partir dos argumentos da linha de comandos.
 * 
 * Junta os argumentos recebidos num único buffer formatado para envio ao servidor.
 * 
 * @param argc Número de argumentos.
 * @param argv Vetor de argumentos da linha de comandos.
 * @return char* Apontador para a mensagem criada (deve ser libertado posteriormente).
 */
char* make_mensagem(int argc, char *argv[]);

/**
 * @brief Envia uma resposta ao cliente através do FIFO de resposta.
 * 
 * @param resposta String contendo a resposta a ser enviada.
 */
void send_response_to_client(const char* resposta);

/**
 * @brief Procura por uma palavra-chave num ficheiro, podendo haver múltiplas ocorrências.
 * 
 * @param path Caminho para o ficheiro.
 * @param keyword Palavra-chave a procurar.
 * @return int Número de ocorrências encontradas.
 */
int search_in_file(const char* path, const char* keyword);

/**
 * @brief Procura por uma única ocorrência de uma palavra-chave num ficheiro.
 * 
 * Retorna assim que a primeira ocorrência for encontrada.
 * 
 * @param path Caminho para o ficheiro.
 * @param keyword Palavra-chave a procurar.
 * @return int 1 se for encontrada uma ocorrência, 0 caso contrário.
 */
int search_in_file_once(char* path, char* keyword);
#endif