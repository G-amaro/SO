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
void add_to_persistence_file(Document* doc, int* exists) {
    // Verifica se o documento já existe (verificação do path, por exemplo)
    if (*exists) {
        printf("Documento com path %s já existente\n", doc->path);
        return;
    }

    // Abre o ficheiro em modo binário para adicionar
    int fd = open(PERSISTENCE_FILE, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para adicionar documento");
        return;
    }

    // Escrever o documento em formato binário
    ssize_t bytes_written = write(fd, doc, sizeof(Document));
    if (bytes_written != sizeof(Document)) {
        perror("Erro ao escrever o documento no ficheiro");
    } else {
        printf("Documento adicionado ao arquivo de persistência.\n");
    }

    close(fd);
}

void remove_from_persistence_file(int id_to_remove) {
    int fd = open(PERSISTENCE_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para leitura");
        return;
    }

    Document docs[100];  // Supondo no máximo 100 documentos
    int count = 0;
    Document doc;

    // Ler todos os documentos
    while (read(fd, &doc, sizeof(Document)) == sizeof(Document)) {
        if (doc.id != id_to_remove) {
            docs[count++] = doc;
        }
    }
    close(fd);

    // Reabrir em modo de escrita (apaga o conteúdo)
    fd = open(PERSISTENCE_FILE, O_WRONLY | O_TRUNC);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para escrita");
        return;
    }

    // Escrever documentos válidos
    for (int i = 0; i < count; i++) {
        if (write(fd, &docs[i], sizeof(Document)) != sizeof(Document)) {
            perror("Erro ao escrever no ficheiro");
            break;
        }
    }

    close(fd);
    printf("Documento com ID %d removido do ficheiro.\n", id_to_remove);
}

void save_documents_to_persistence() {
    int fd = open(PERSISTENCE_FILE, O_WRONLY | O_TRUNC);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para escrita");
        return;
    }

    Document* current = document_list;
    while (current) {
        write(fd, current, sizeof(Document));  // Escrever o documento binário
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

    // O arquivo será limpo ao ser aberto com O_TRUNC
    printf("Arquivo de persistência limpo com sucesso!\n");

    close(fd);
}
int load_documents_and_get_max_id() {
    int max_id = 0;
    int fd = open(PERSISTENCE_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para leitura");
        return -1;
    }

    Document doc;
    while (read(fd, &doc, sizeof(Document)) > 0) {
        if (doc.id > max_id) {
            max_id = doc.id;
        }

        Document* new_doc = (Document*)malloc(sizeof(Document));
        if (!new_doc) {
            perror("Erro ao alocar memória para documento");
            close(fd);
            return -1;
        }

        *new_doc = doc;  // Copiar os dados do documento
        new_doc->next = document_list;
        document_list = new_doc;
    }

    close(fd);
    return max_id;
} 