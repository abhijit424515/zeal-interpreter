if [ "$#" -eq 0 ]; then
    echo "Usage: ./loop-v.sh <lab>"
    exit 1
fi

LAB=$1
valid=(8 18 16)
invalid=(7 5 4)

make

# SCAN and PARSE

for level in {1..3}
do
    for num in $(seq 1 ${valid[level-1]})
    do
        ./scripts/v.sh $level $num $LAB "--show-tokens"
    done
done

# AST

for level in {1..3}
do
    for num in $(seq 1 ${valid[level-1]})
    do
        ./scripts/v.sh $level $num $LAB "--show-ast"
    done
done

# TAC

for level in {1..3}
do
    for num in $(seq 1 ${valid[level-1]})
    do
        ./scripts/v.sh $level $num $LAB "--show-tac"
    done
done

# ---

make clean