#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista_ligada.h"

Document* document_list = NULL;

char* add_document(char* title, char* authors, char* year, char* path, int* next_id) {
    char* resposta = (char*)malloc(512 * sizeof(char)); 
    if (!resposta) {
        perror("Erro ao alocar memória para resposta");
        return NULL;
    }

    Document* new_doc = (Document*)malloc(sizeof(Document));
    if (!new_doc) {
        perror("Erro ao alocar memória para documento");
        snprintf(resposta, 512, "Não foi possível indexar o documento.");
        return resposta;
    }

    Document* current = document_list;
    printf("Procurando documento com path = %s\n", path);  

    while (current != NULL) {
        if (strcmp(current->path, path) == 0) {
            snprintf(resposta, 512, "Documento com path = %s já existe na lista ligada", current->path);
            printf("%s\n", resposta);
            return resposta;
        }
        current = current->next;
    }

    new_doc->id = *next_id; // Usar o valor de next_id
    (*next_id)++;  // Incrementar next_id

    strncpy(new_doc->title, title, 200);
    new_doc->title[200] = '\0';
    strncpy(new_doc->authors, authors, 200);
    new_doc->authors[200] = '\0';
    strncpy(new_doc->year, year, 4);
    new_doc->year[4] = '\0';
    strncpy(new_doc->path, path, 64);
    new_doc->path[64] = '\0';
    new_doc->next = document_list;
    document_list = new_doc;

    snprintf(resposta, 512, "Documento indexado: ID=%d, Title=%s, Authors=%s, Year=%s, Path=%s", new_doc->id, new_doc->title, new_doc->authors, new_doc->year, new_doc->path);
    printf("%s\n", resposta);
    return resposta;
}


char* delete_document(char* key) {
    char* resposta = (char*)malloc(512 * sizeof(char));
    if (!resposta) {
        perror("Erro ao alocar memória para resposta");
        return NULL;
    }

    int id_to_delete = atoi(key);
    Document* current = document_list;
    Document* previous = NULL;

    while (current != NULL) {
        if (current->id == id_to_delete) {
            if (previous == NULL) {
                document_list = current->next;
            } else {
                previous->next = current->next;
            }
            snprintf(resposta, 512, "Documento com ID=%d eliminado.", current->id);
            printf("%s\n", resposta);
            free(current);
            return resposta;
        }
        previous = current;
        current = current->next;
    }

    snprintf(resposta, 512, "Documento com ID=%d não encontrado.", id_to_delete);
    printf("%s\n", resposta);
    return resposta;
}

char* search_document(char* key) {
    char* resposta = (char*)malloc(512 * sizeof(char));
    if (!resposta) {
        perror("Erro ao alocar memória para resposta");
        return NULL;
    }

    int id_to_search = atoi(key);
    Document* current = document_list;
    

    while (current != NULL) {
        
        if (current->id == id_to_search) {
            
            snprintf(resposta, 512, "Documento com ID=%d consultado.\n  -title: %s;\n  -authors: %s;\n  -year: %s;\n  -path: %s;\n", 
                     current->id, current->title, current->authors, current->year, current->path);
            printf("%s\n", resposta);
            return resposta;
        }
        current = current->next;
    }

    // Se não encontrar o documento, retorna uma mensagem de erro
    snprintf(resposta, 512, "Documento com ID=%d não encontrado", id_to_search);
    printf("%s\n", resposta);  
    return resposta;
}

void list_documents() {
    Document* current = document_list;
    if (current == NULL) {
        printf("Nenhum documento na lista.\n");
        return;
    }

    printf("Lista de documentos:\n");
    printf("----------------------\n");
    
    while (current != NULL) {
        printf("ID: %d\n", current->id);
        printf("Title: %s\n", current->title);
        printf("Authors: %s\n", current->authors);
        printf("Year: %s\n", current->year);
        printf("Path: %s\n", current->path);
        printf("----------------------\n");
        
        current = current->next;
    }
}


void free_documents() {
    Document* current = document_list;
    Document* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    document_list = NULL;
}
