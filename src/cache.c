#include <stdio.h>
#include <stdlib.h>
#include "cache.h"


typedef struct CacheEntry {
    Document *doc;
    struct CacheEntry *next;
} CacheEntry;

static CacheEntry *cache_head = NULL;
static int cache_size = 0;
static int cache_max_size = 0;

void init_cache(int size) {
    cache_max_size = size;
    cache_size = 0;
    cache_head = NULL;
}

void free_cache() {
    CacheEntry *current = cache_head;
    while (current != NULL) {
        CacheEntry *temp = current;
        current = current->next;
        free(temp);
    }
    cache_head = NULL;
    cache_size = 0;
}

Document* find_in_cache(int id) {
    CacheEntry *current = cache_head;
    while (current != NULL) {
        if (current->doc->id == id) {
            return current->doc;
        }
        current = current->next;
    }
    return NULL;
}

void remove_last_from_cache() {
    if (cache_head == NULL) return;

    CacheEntry *current = cache_head;
    CacheEntry *prev = NULL;

    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    if (prev != NULL) {
        prev->next = NULL;
    } else {
        cache_head = NULL;
    }

    free(current);
    cache_size--;
}

void add_to_cache(Document *doc) {
    if (find_in_cache(doc->id) != NULL) {

        return;
    }

    CacheEntry *new_entry = malloc(sizeof(CacheEntry));
    if (new_entry == NULL) {
        perror("Erro a alocar memória para cache");
        return;
    }
    new_entry->doc = doc;
    new_entry->next = cache_head;
    cache_head = new_entry;
    cache_size++;
    if (cache_size > cache_max_size) {
        remove_last_from_cache();
    }
}

void remove_from_cache(int id) {
    CacheEntry* current = cache_head;
    CacheEntry* previous = NULL;

    while (current != NULL) {
        if (current->doc->id == id) {
            if (previous == NULL) {
                cache_head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);  // Só libertar o nó da cache, NÃO o document!!
            return;
        }
        previous = current;
        current = current->next;
    }
}



void list_documents_cache() {
    CacheEntry* current = cache_head;
    if (current == NULL) {
        printf("Nenhum documento na cache.\n");
        return;
    }

    printf("Lista de documentos da cache:\n");
    printf("----------------------\n");
    
    while (current != NULL) {
        printf("ID: %d\n", current->doc->id);
        printf("Title: %s\n", current->doc->title);
        printf("Authors: %s\n", current->doc->authors);
        printf("Year: %s\n", current->doc->year);
        printf("Path: %s\n", current->doc->path);
        printf("----------------------\n");
        
        current = current->next;
    }
}
