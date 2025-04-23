#ifndef MANIPULACAO_LISTALIGADA_H
#define MANIPULACAO_LISTALIGADA_H

// Definição da estrutura Document
typedef struct Document {
    int id;
    char title[201];
    char authors[201];
    char year[5];
    char path[65];
    struct Document* next;
} Document;

// Funções públicas da manipulação de documentos
char* add_document(char* title, char* authors, char* year, char* path);
char* delete_document(char* key);
char* search_document(char* key);
void free_documents();

#endif // MANIPULACAO_LISTALIGADA_H
