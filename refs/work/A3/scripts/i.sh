LAB=$3
IR=$4
CDIR=../../ps/$LAB-resources

./sclp $CDIR/example-programs/Level-$1-invalid-test-cases/l$1-invalid-exmp$2.c $IR -d > t1
echo $? > c1
$CDIR/reference-implementations/$LAB-sclp $CDIR/example-programs/Level-$1-invalid-test-cases/l$1-invalid-exmp$2.c $IR -d > t2
echo $? > c2
clear
diff -Bw c1 c2 >> ./scripts/c

echo "---- L: " $1 "T: " $2 "----"
diff -Bw c1 c2

cdif=$(diff -Bw c1 c2)

if [ -n "$cdif" ]; then
    echo "CDIF: " $cdif >> ./scripts/dif
    exit
fi

z=$(cat c1)
if [[ z -eq 0 ]]; then 
    diff -Bw t1 t2 >> ./scripts/t
    odif=$(diff -Bw t1 t2)
    if [ -n "$odif" ]; then
        echo "ODIF: " $odif >> ./scripts/dif
    fi
fi

rm -f t1 t2 c1 c2