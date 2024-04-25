if [ "$#" -eq 0 ]; then
    echo "Usage: ./loop-i.sh <lab>"
    exit 1
fi

LAB=$1
invalid=(7 5 4 5)
len=${#valid[@]}

rm -f ./scripts/t ./scripts/c
make

# SCAN and PARSE

for ((level=1; level <= len; level++))
do
    for num in $(seq 1 ${invalid[level-1]})
    do
        ./scripts/i.sh $level $num $LAB "--show-tokens"
    done
done

# AST

for ((level=1; level <= len; level++))
do
    for num in $(seq 1 ${invalid[level-1]})
    do
        ./scripts/i.sh $level $num $LAB "--show-ast"
    done
done

# TAC

for ((level=1; level <= len; level++))
do
    for num in $(seq 1 ${invalid[level-1]})
    do
        ./scripts/i.sh $level $num $LAB "--show-tac"
    done
done

# RTL

for ((level=1; level <= len; level++))
do
    for num in $(seq 1 ${invalid[level-1]})
    do
        ./scripts/i.sh $level $num $LAB "--show-rtl --suppress-comments"
    done
done

# ---

make clean