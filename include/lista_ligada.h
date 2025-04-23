#ifndef LISTA_LIGADA_H
#define LISTA_LIGADA_H

// Definição da estrutura Document
typedef struct Document {
    int id;
    char title[201];
    char authors[201];
    char year[5];
    char path[65];
    struct Document* next;
} Document;

extern Document* document_list;

// Funções públicas da manipulação de documentos
char* add_document(char* title, char* authors, char* year, char* path, int* next_id);
char* delete_document(char* key);
char* search_document(char* key);
void list_documents();
void free_documents();

#endif 
