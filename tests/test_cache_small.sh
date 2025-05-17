#!/bin/bash

DOCS_DIR="docs/my docs"
SERVER="./bin/dserver"
CLIENT="./bin/dclient"
KEYWORD="praia"
EXTRA="erase"

CACHE_SIZES=(1 5 8)
RESULTS=""

for size in "${CACHE_SIZES[@]}"; do
    echo "--------------------------------------------------"
    echo "Inicializando servidor com cache_size = $size"

    # Iniciar servidor em background
    "$SERVER" "$DOCS_DIR" "$size" "$EXTRA" &
    SERVER_PID=$!

    # Esperar o servidor iniciar
    sleep 1

    echo "Indexando ficheiros..."
    for i in {1..8}; do
        FILE="file${i}.txt"
        TITLE="file${i}"
        "$CLIENT" -a "$TITLE" "Autor" "2020" "$FILE" > /dev/null
    done

    echo "Realizando pesquisa com cache_size = $size..."
    # Medir tempo de execução e salvar em variável
    TIMEFORMAT="%R"
    exec_time=$( { time "$CLIENT" -s "$KEYWORD" 4 > /dev/null; } 2>&1 )

    # Parar servidor
    "$CLIENT" -f > /dev/null
    wait $SERVER_PID

    # Guardar resultado formatado
    RESULTS+="cache_size = $size : ${exec_time}s"$'\n'
done

echo ""
echo "================== TEMPOS DE EXECUÇÃO =================="
echo "$RESULTS"
