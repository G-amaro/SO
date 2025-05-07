#ifndef CONSULTAR_H
#define CONSULTAR_H

/**
 * @brief Executa a operação de consulta de um documento com base numa chave.
 * 
 * A chave pode ser um título ou um ID. A função procura a informação correspondente
 * e trata da resposta adequada ao cliente.
 * 
 * @param key Chave de pesquisa (título ou ID do documento).
 */
void operacao_consultar(char* key);

#endif
