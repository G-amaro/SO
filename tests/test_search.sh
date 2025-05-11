#!/bin/bash

KEYWORD="huge"

PROCESS_COUNTS=(1 2 4 8)
DOCS_DIR="./docs/Gdataset"
SERVER="./bin/dserver"
EXTRA="erase"

RESULTS=""

echo "Teste de pesquisa concorrente para a palavra-chave '$KEYWORD'"

for n in "${PROCESS_COUNTS[@]}"; do
    TIMEFORMAT="%R"
    exec_time=$(time ( ./bin/dclient -s "$KEYWORD" "$n" > /dev/null ) 2>&1)

    RESULTS+="Pesquisa com $n processos: ${exec_time}s"$'\n'
done

echo ""
echo "================== TEMPOS DE EXECUÇÃO =================="
echo "$RESULTS"
