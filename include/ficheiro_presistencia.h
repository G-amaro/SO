#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "lista_ligada.h"

/// Caminho para o ficheiro de persistência principal.
#define PERSISTENCE_FILE "/tmp/documents.bin"
/// Caminho para o ficheiro temporário usado durante operações de escrita.
#define TEMP_FILE "/tmp/temp.bin"

/**
 * @brief Inicializa o ficheiro de persistência.
 * 
 * Cria o ficheiro de persistência caso ainda não exista.
 */
void init_persistence_file();

/**
 * @brief Guarda todos os documentos atuais no ficheiro de persistência.
 * 
 * Os documentos da estrutura em memória são escritos no ficheiro binário.
 */
void save_documents_to_persistence();

/**
 * @brief Lista os documentos atualmente armazenados no ficheiro de persistência.
 * 
 * Lê diretamente do ficheiro binário e imprime as informações dos documentos.
 */
void list_documents_in_persistence();

/**
 * @brief Limpa (remove todo o conteúdo de) o ficheiro de persistência.
 */
void clear_persistence_file();

/**
 * @brief Carrega os documentos do ficheiro de persistência para a estrutura em memória.
 * 
 * Lê o ficheiro binário e preenche a lista ligada com os documentos.
 */
void load_documents();



#endif