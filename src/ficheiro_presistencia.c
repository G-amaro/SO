#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "ficheiro_presistencia.h"
#include "lista_ligada.h"



void init_persistence_file() {
    int fd = open(PERSISTENCE_FILE, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Erro ao criar o ficheiro de persistência");
        exit(1);
    }
    close(fd);
}



void save_documents_to_persistence() {
    int fd = open(PERSISTENCE_FILE, O_WRONLY | O_TRUNC);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para escrita");
        return;
    }

    Document* current = document_list;
    while (current) {
        write(fd, current, sizeof(Document));  
        current = current->next;
    }

    close(fd);
}

void list_documents_in_persistence() {
    int fd = open(PERSISTENCE_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para leitura");
        return;
    }

    Document doc;
    printf("Lista de documentos no arquivo de persistência:\n");
    printf("-------------------------------------------------\n");

    while (read(fd, &doc, sizeof(Document)) > 0) {
        printf("ID: %d\n", doc.id);
        printf("Title: %s\n", doc.title);
        printf("Authors: %s\n", doc.authors);
        printf("Year: %s\n", doc.year);
        printf("Path: %s\n", doc.path);
        printf("----------------------------------------\n");
    }

    close(fd);
}

void clear_persistence_file() {
    int fd = open(PERSISTENCE_FILE, O_WRONLY | O_TRUNC);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo de persistência para limpeza");
        return;
    }


    printf("Arquivo de persistência limpo com sucesso!\n");

    close(fd);
}

void load_documents() {
    int fd = open(PERSISTENCE_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para leitura");
    }

    Document doc;
    while (read(fd, &doc, sizeof(Document)) > 0) {
        
        Document* new_doc = (Document*)malloc(sizeof(Document));
        if (!new_doc) {
            perror("Erro ao alocar memória para documento");
            close(fd);
                }

        *new_doc = doc;  
        new_doc->next = document_list;
        document_list = new_doc;
    }

    close(fd); 
}
