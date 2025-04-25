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
	@mkdir -p src include obj/operações bin tmp
	@echo "Diretórios src, include, obj/operações, bin, tmp criados com sucesso."

# Compilar bin/dserver
bin/dserver: obj/dserver.o obj/lista_ligada.o obj/utils.o obj/ficheiro_presistencia.o obj/operações/adicionar.o obj/operações/contar.o obj/operações/consultar.o obj/operações/eliminar.o obj/operações/pesquisar.o
	$(CC) $(LDFLAGS) $^ -o $@
	@echo "dserver compilado com sucesso!"

# Compilar bin/dclient
bin/dclient: obj/dclient.o obj/utils.o
	$(CC) $(LDFLAGS) $^ -o $@
	@echo "dclient compilado com sucesso!"

# Compilar os arquivos .o a partir dos .c
# Regra para compilar arquivos dentro de src/ e garantir que os arquivos .o vão para obj/
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o obj/$(@F)
	@echo "$< compilado com sucesso!"

# Compilar arquivos dentro de src/operações/ e garantir que os arquivos .o vão para obj/operações/
obj/operações/%.o: src/operações/%.c
	$(CC) $(CFLAGS) -c $< -o obj/operações/$(@F)
	@echo "$< compilado com sucesso!"

# Limpar arquivos compilados
clean:
	rm -f obj/* tmp/* bin/*
	@echo "Arquivos compilados limpos com sucesso."
