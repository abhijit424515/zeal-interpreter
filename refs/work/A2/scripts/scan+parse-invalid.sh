CDIR=../../ps/A2-resources
# make
./sclp $CDIR/example-programs/Level-$1-invalid-test-cases/l$1-invalid-exmp$2.c --show-tokens --sa-parse -d > t1
echo $? > c1
# make clean
$CDIR/reference-implementations/A2-sclp $CDIR/example-programs/Level-$1-invalid-test-cases/l$1-invalid-exmp$2.c --show-tokens --sa-parse -d > t2
echo $? > c2
clear
# diff -Bw t1 t2
echo $1 $2 "----"
diff -Bw c1 c2 >> ./scripts/c
rm -f t1 t2 c1 c2