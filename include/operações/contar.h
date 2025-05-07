#ifndef CONTAR_H
#define CONTAR_H

/**
 * @brief Executa a operação de contagem de ocorrências de uma palavra-chave.
 * 
 * A função pesquisa a palavra-chave num ficheiro específico ou dentro de uma pasta,
 * dependendo dos argumentos fornecidos.
 * 
 * @param filename Nome do ficheiro onde será feita a pesquisa (pode ser NULL).
 * @param folder Nome da pasta onde procurar os ficheiros (pode ser NULL).
 * @param keyword Palavra-chave a procurar.
 */
void operacao_contar(char* filename, char* folder , char* keyword);

#endif