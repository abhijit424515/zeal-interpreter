valid=(8 18 16)
invalid=(7 5 4)

make

for level in {1..3}
do
    for num in $(seq 1 ${valid[level-1]})
    do
        ./scripts/scan+parse-valid.sh $level $num
    done
done

for level in {1..3}
do
    for num in $(seq 1 ${invalid[level-1]})
    do
        ./scripts/scan+parse-invalid.sh $level $num
    done
done

for level in {1..3}
do
    for num in $(seq 1 ${valid[level-1]})
    do
        ./scripts/ast-valid.sh $level $num
    done
done

# for level in {1..3}
# do
#     for num in $(seq 1 ${invalid[level-1]})
#     do
#         ./scripts/ast-invalid.sh $level $num
#     done
# done

make clean