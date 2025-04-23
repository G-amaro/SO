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

void add_to_persistence_file(Document* doc) {
    // Abrir o arquivo de persistência em modo de leitura
    int fd = open(PERSISTENCE_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para leitura");
        return;
    }

    char line[512];
    int exists = 0;

    // Verificar se o documento já existe no arquivo de persistência
    while (read(fd, line, sizeof(line)) > 0) {
        int doc_id;
        char path[64];
        // Ler os dados do arquivo
        sscanf(line, "%d|%*[^|]|%*[^|]|%*[^|]|%63[^\n]", &doc_id, path);

        // Verificar se o caminho já existe no arquivo de persistência
        if (strcmp(path, doc->path) == 0) {
            exists = 1;
            break;
        }
    }

    close(fd);  // Fechar o arquivo após a leitura

    // Se o documento já existe, não adicionamos
    if (exists) {
        printf("Documento com o mesmo caminho já existe no arquivo de persistência.\n");
        return;
    }

    // Se o documento não existir, adicionar ao arquivo
    fd = open(PERSISTENCE_FILE, O_WRONLY | O_APPEND | O_CREAT, 0666);  // Abrir para adicionar
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para adicionar documento");
        return;
    }

    // Escrever os dados no arquivo
    dprintf(fd, "%d|%s|%s|%s|%s\n", doc->id, doc->title, doc->authors, doc->year, doc->path);
    close(fd);  // Fechar o arquivo após escrever
    printf("Documento adicionado ao arquivo de persistência.\n");
}

void remove_from_persistence_file(int doc_id) {
    int fd = open(PERSISTENCE_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para leitura");
        return;
    }

    int temp_fd = open("/tmp/temp.db", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (temp_fd == -1) {
        perror("Erro ao criar ficheiro temporário");
        close(fd);
        return;
    }

    char line[512];
    int current_id;
    ssize_t bytes_read;

    // Ler o arquivo de persistência linha por linha
    while ((bytes_read = read(fd, line, sizeof(line))) > 0) {
        char *line_end = strchr(line, '\n');
        if (line_end != NULL) {
            *line_end = '\0';  // Substituir o '\n' no final da linha por '\0'
        }

        sscanf(line, "%d", &current_id);

        // Se o ID não for o que queremos remover, escrever a linha no arquivo temporário
        if (current_id != doc_id) {
            write(temp_fd, line, bytes_read);
            write(temp_fd, "\n", 1);  // Adicionar nova linha no arquivo temporário
        }
    }

    if (bytes_read == -1) {
        perror("Erro ao ler o ficheiro de persistência");
    }

    close(fd);     // Fechar o arquivo de persistência
    close(temp_fd); // Fechar o arquivo temporário

    // Remover o arquivo original
    if (remove(PERSISTENCE_FILE) != 0) {
        perror("Erro ao remover o arquivo original");
        return;
    }

    // Renomear o arquivo temporário para o arquivo de persistência
    if (rename("/tmp/temp.db", PERSISTENCE_FILE) != 0) {
        perror("Erro ao renomear o arquivo temporário");
    } else {
        printf("Documento com ID=%d eliminado.\n", doc_id);
    }
}



void load_documents_from_persistence() {
    int fd = open(PERSISTENCE_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para leitura");
        return;
    }

    char line[512];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, line, sizeof(line) - 1)) > 0) {
        line[bytes_read] = '\0';  // Certifique-se de que a string termina em '\0'

        char* token;
        Document* doc = (Document*)malloc(sizeof(Document));
        if (!doc) {
            perror("Erro ao alocar memória para documento");
            close(fd);
            return;
        }

        token = strtok(line, "|");
        if (token) {
            doc->id = atoi(token); // ID
            token = strtok(NULL, "|");
            if (token) {
                strncpy(doc->title, token, 200);  // Título
                doc->title[200] = '\0';
                token = strtok(NULL, "|");
                if (token) {
                    strncpy(doc->authors, token, 200);  // Autores
                    doc->authors[200] = '\0';
                    token = strtok(NULL, "|");
                    if (token) {
                        strncpy(doc->year, token, 4);  // Ano
                        doc->year[4] = '\0';
                        token = strtok(NULL, "|");
                        if (token) {
                            strncpy(doc->path, token, 64);  // Caminho
                            doc->path[64] = '\0';
                            doc->next = document_list;
                            document_list = doc;  // Adiciona à lista ligada
                        }
                    }
                }
            }
        }
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
        dprintf(fd, "%d|%s|%s|%s|%s\n", current->id, current->title, current->authors, current->year, current->path);
        current = current->next;
    }

    close(fd);
}

void list_documents_in_persistence() {
    FILE* file = fopen(PERSISTENCE_FILE, "r");
    if (!file) {
        perror("Erro ao abrir o ficheiro de persistência para leitura");
        return;
    }

    char line[512];
    printf("Lista de documentos no arquivo de persistência:\n");
    printf("-------------------------------------------------\n");

    while (fgets(line, sizeof(line), file)) {
        Document doc;
        int id;
        sscanf(line, "%d|%200[^|]|%200[^|]|%4[^|]|%64[^\n]",
               &id, doc.title, doc.authors, doc.year, doc.path);

        // Exibir os detalhes do documento
        printf("ID: %d\n", id);
        printf("Title: %s\n", doc.title);
        printf("Authors: %s\n", doc.authors);
        printf("Year: %s\n", doc.year);
        printf("Path: %s\n", doc.path);
        printf("----------------------------------------\n");
    }

    fclose(file);
}

void clear_persistence_file() {
    FILE* file = fopen(PERSISTENCE_FILE, "w");  // Abre o arquivo no modo "w" para sobrescrever
    if (!file) {
        perror("Erro ao abrir o arquivo de persistência para limpeza");
        return;
    }

    // O arquivo é automaticamente limpo ao ser aberto em modo "w"
    printf("Arquivo de persistência limpo com sucesso!\n");

    fclose(file);  // Fecha o arquivo
} 

int get_max_id_from_persistence() {
    int max_id = 1;  // Inicializa com 0
    int fd = open(PERSISTENCE_FILE, O_RDONLY);  // Abre o arquivo para leitura
    if (fd == -1) {
        perror("Erro ao abrir o ficheiro de persistência para leitura");
        return -1;  // Retorna erro
    }

    char buffer[512];  // Buffer para armazenar os dados lidos
    int bytes_read;
    char line[512];
    int line_pos = 0;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {  // Lê dados do arquivo
        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {  // Detecta o fim de uma linha
                line[line_pos] = '\0';  // Termina a linha com o caracter nulo

                // Processa a linha lida
                int id;
                char* ptr = line;
                if (sscanf(ptr, "%d|", &id) == 1) {
                    if (id > max_id) {
                        max_id = id;  
                    }
                }

                // Reinicia a posição da linha
                line_pos = 0;
            } else {
                if (line_pos < sizeof(line) - 1) {
                    line[line_pos++] = buffer[i];  // Armazena o caractere no buffer da linha
                }
            }
        }
    }

    close(fd);  // Fecha o arquivo
    return max_id;  // Retorna o maior ID encontrado
}
