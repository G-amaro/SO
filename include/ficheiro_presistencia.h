#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "lista_ligada.h"

#define PERSISTENCE_FILE "/tmp/documents.bin"
#define TEMP_FILE "/tmp/temp.bin"

void init_persistence_file();

void add_to_persistence_file(Document* doc, int* exists) ;

void remove_from_persistence_file(int id);

void save_documents_to_persistence();

void list_documents_in_persistence();

void clear_persistence_file();

void load_documents();

int get_last_document_id();

#endif