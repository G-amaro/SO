#ifndef PESQUISAR_H
#define PESQUISAR_H

/**
 * @brief Executa a operação de pesquisa de uma palavra-chave em documentos.
 * 
 * A função interpreta a mensagem e realiza a pesquisa pela palavra-chave nos documentos
 * presentes na pasta indicada.
 * 
 * @param mensagem String contendo a palavra-chave e possivelmente outras instruções.
 * @param folder Caminho da pasta onde os documentos devem ser pesquisados.
 */
void operacao_pesquisar(char* mensagem, char* folder);

#endif
