#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "lista_ligada.h"

#define PERSISTENCE_FILE "/tmp/documents.bin"
#define TEMP_FILE "/tmp/temp.bin"

void init_persistence_file();

void save_documents_to_persistence();

void list_documents_in_persistence();

void clear_persistence_file();

void load_documents();



#endif