#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "lista_ligada.h"

#define PERSISTENCE_FILE "/tmp/documentos.db"


void init_persistence_file();

void add_to_persistence_file(Document* doc) ;

void remove_from_persistence_file(int id);

void load_documents_from_persistence();

void save_documents_to_persistence();

void list_documents_in_persistence();

void clear_persistence_file();

int get_max_id_from_persistence();
#endif