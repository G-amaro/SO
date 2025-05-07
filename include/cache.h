#ifndef CACHE_H
#define CACHE_H

#include "lista_ligada.h"

/**
 * @brief Inicializa a cache com um tamanho máximo especificado.
 * 
 * @param size Tamanho máximo da cache (número de documentos).
 */
void init_cache(int size);

/**
 * @brief Liberta toda a memória associada à cache.
 */
void free_cache();

/**
 * @brief Procura um documento na cache pelo seu ID.
 * 
 * @param id Identificador único do documento.
 * @return Apontador para o documento, se encontrado; caso contrário, NULL.
 */
Document* find_in_cache(int id);

/**
 * @brief Remove o último documento da cache (usado em política de substituição, como LRU).
 */
void remove_last_from_cache();

/**
 * @brief Adiciona um novo documento à cache.
 * 
 * @param doc Apontador para o documento a adicionar.
 */
void add_to_cache(Document *doc);

/**
 * @brief Remove um documento da cache com base no seu ID.
 * 
 * @param id Identificador único do documento a remover.
 */
void remove_from_cache(int id);

/**
 * @brief Lista todos os documentos atualmente na cache.
 */
void list_documents_cache();

#endif