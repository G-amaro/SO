#ifndef LISTA_LIGADA_H
#define LISTA_LIGADA_H

/**
 * @brief Estrutura que representa um documento.
 * 
 * Contém os campos de identificação, título, autores, ano, caminho e um apontador para o próximo documento (lista ligada).
 */
typedef struct Document {
    int id;
    char title[201];
    char authors[201];
    char year[5];
    char path[65];
    struct Document* next;
} Document;


/// Apontador global para o início da lista de documentos.
extern Document* document_list;

/**
 * @brief Obtém o ID do primeiro documento da lista.
 * 
 * @return int ID do primeiro documento, ou -1 se a lista estiver vazia.
 */
int get_first_document_id();

/**
 * @brief Adiciona um novo documento à lista.
 * 
 * @param title Título do documento.
 * @param authors Autores do documento.
 * @param year Ano de publicação.
 * @param path Caminho para o ficheiro.
 * @param next_id ID a atribuir ao novo documento.
 * @param exists Ponteiro para flag que indica se o documento já existia.
 * @return char* Mensagem de sucesso ou erro.
 */
char* add_document(char* title, char* authors, char* year, char* path, int next_id, int* exists);

/**
 * @brief Remove um documento da lista com base numa chave (título ou ID).
 * 
 * @param key Chave de pesquisa (título ou ID em string).
 * @param exist Ponteiro para flag que indica se o documento foi encontrado.
 * @return char* Mensagem de sucesso ou erro.
 */
char* delete_document(char* key, int* exist);

/**
 * @brief Pesquisa um documento na lista com base numa chave (título ou ID).
 * 
 * @param key Chave de pesquisa.
 * @return char* Informação do documento encontrado ou mensagem de erro.
 */
char* search_document(char* key);

/**
 * @brief Lista todos os documentos presentes na lista ligada.
 */
void list_documents();

/**
 * @brief Obtém um documento da lista pelo seu ID.
 * 
 * @param id Identificador do documento.
 * @return Document* Apontador para o documento encontrado, ou NULL se não existir.
 */
Document* get_doc_by_id(int id); 
/**
 * @brief Liberta toda a memória ocupada pela lista de documentos.
 */
void free_documents();

#endif 
