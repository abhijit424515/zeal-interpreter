if [ "$#" -eq 0 ]; then
    echo "Usage: ./loop-v.sh <lab>"
    exit 1
fi

LAB=$1
valid=(8 18 16 25)
len=${#valid[@]}

rm -f ./scripts/t ./scripts/c
make

# SCAN and PARSE

for ((level=1; level <= len; level++))
do
    for num in $(seq 1 ${valid[level-1]})
    do
        ./scripts/v.sh $level $num $LAB "--show-tokens"
    done
done

# AST

for ((level=1; level <= len; level++))
do
    for num in $(seq 1 ${valid[level-1]})
    do
        ./scripts/v.sh $level $num $LAB "--show-ast"
    done
done

# TAC

for ((level=1; level <= len; level++))
do
    for num in $(seq 1 ${valid[level-1]})
    do
        ./scripts/v.sh $level $num $LAB "--show-tac"
    done
done

# RTL

for ((level=1; level <= len; level++))
do
    for num in $(seq 1 ${valid[level-1]})
    do
        ./scripts/v.sh $level $num $LAB "--show-rtl --suppress-comments"
    done
done

# ---

make clean