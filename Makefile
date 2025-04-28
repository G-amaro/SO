CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS = #-fsanitize=address

all: folders dserver dclient

dserver: bin/dserver

dclient: bin/dclient

folders:
	@mkdir -p src include obj/operações bin tmp

bin/dserver: obj/dserver.o obj/lista_ligada.o obj/utils.o obj/ficheiro_presistencia.o obj/cache.o obj/operações/adicionar.o obj/operações/contar.o obj/operações/consultar.o obj/operações/eliminar.o obj/operações/pesquisar.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/dclient: obj/dclient.o obj/utils.o
	$(CC) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o obj/$(@F)
	@echo "$< compilado com sucesso!"

obj/operações/%.o: src/operações/%.c
	$(CC) $(CFLAGS) -c $< -o obj/operações/$(@F)
	@echo "$< compilado com sucesso!"

clean:
	rm -rf obj tmp bin
	@echo "Arquivos compilados limpos com sucesso."
