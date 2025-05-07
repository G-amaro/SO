#ifndef ELIMINAR_H
#define ELIMINAR_H

/**
 * @brief Executa a operação de eliminação de um documento com base na mensagem recebida.
 * 
 * A mensagem deve conter a chave (ID ou título) do documento a eliminar. A função trata 
 * da remoção do documento das estruturas de dados e/ou ficheiros persistentes.
 * 
 * @param mensagem String contendo a informação necessária para identificar o documento a eliminar.
 */
void operacao_eliminar(char* mensagem);

#endif
