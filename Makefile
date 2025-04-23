# Compilador e flags
CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS =

# Diretórios
all: folders dserver dclient

dserver: bin/dserver
	@echo "dserver compilado com sucesso!"

dclient: bin/dclient
	@echo "dclient compilado com sucesso!"

# Criar diretórios
folders:
	@mkdir -p src include obj bin tmp
	@echo "Diretórios src, include, obj, bin e tmp criados com sucesso."

# Compilar bin/dserver
bin/dserver: obj/dserver.o obj/lista_ligada.o obj/utils.o obj/ficheiro_presistencia.o
	$(CC) $(LDFLAGS) $^ -o $@
	@echo "dserver compilado com sucesso!"

# Compilar bin/dclient
bin/dclient: obj/dclient.o obj/utils.o
	$(CC) $(LDFLAGS) $^ -o $@
	@echo "dclient compilado com sucesso!"

# Compilar os arquivos .o a partir dos .c
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "$< compilado com sucesso!"

# Limpar arquivos compilados
clean:
	rm -f obj/* tmp/* bin/*
	@echo "Arquivos compilados limpos com sucesso."
