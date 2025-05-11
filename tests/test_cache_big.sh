#!/bin/bash

DOCS_DIR="docs/Gdataset"
SERVER="./bin/dserver"
CLIENT="./bin/dclient"
KEYWORD="praia"
EXTRA="erase"

CACHE_SIZES=(1 5 10 20)
RESULTS=""

for size in "${CACHE_SIZES[@]}"; do
    echo "--------------------------------------------------"
    echo "Inicializando servidor com cache_size = $size"

    # Iniciar servidor em background
    $SERVER "$DOCS_DIR" "$size" "$EXTRA" &
    SERVER_PID=$!

    sleep 1

    echo "Indexação de documentos..."
    for file in "$DOCS_DIR"/*.txt; do
        title=$(basename "$file" .txt)
        $CLIENT -a "$title" "Autor" "2020" "$(basename "$file")" > /dev/null
    done

    echo "Realizando pesquisa com cache_size = $size..."
    # Medir tempo de execução e salvar em variável
    TIMEFORMAT="%R"
    exec_time=$( { time $CLIENT -s "$KEYWORD" 4 > /dev/null; } 2>&1 )



    # Parar servidor
    $CLIENT -f > /dev/null
    wait $SERVER_PID

    # Guardar resultado formatado
    RESULTS+="cache_size = $size : ${exec_time}s"$'\n'
done

echo ""
echo "================== TEMPOS DE EXECUÇÃO =================="
echo "$RESULTS"
