LAB=$3
IR=$4
CDIR=../../ps/$LAB-resources

./sclp $CDIR/example-programs/Level-$1-test-cases/l$1-exmp$2.c  $IR -d > t1
echo $? > c1
$CDIR/reference-implementations/$LAB-sclp $CDIR/example-programs/Level-$1-test-cases/l$1-exmp$2.c $IR -d > t2
echo $? > c2
clear
diff -Bw t1 t2 >> ./scripts/t
diff -Bw c1 c2 >> ./scripts/c

echo "---- L: " $1 "T: " $2 "----"
odif=$(diff -Bw t1 t2)
cdif=$(diff -Bw c1 c2)

if [ -n "$odif" ]; then
    echo "ODIF: " $odif
fi

if [ -n "$cdif" ]; then
    echo "CDIF: " $cdif
fi

rm -f t1 t2 c1 c2