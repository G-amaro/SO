#ifndef CACHE_H
#define CACHE_H

#include "lista_ligada.h"

void init_cache(int size);
void free_cache();

Document* find_in_cache(int id);
void remove_last_from_cache();
void add_to_cache(Document *doc);
void remove_from_cache(int id);
void list_documents_cache();

#endif
