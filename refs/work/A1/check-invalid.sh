CDIR=../../ps/A1-resources

make && ./sclp --show-tokens $CDIR/example-programs/Level-$1-invalid-test-cases/l$1-invalid-exmp$2.c > t1
echo $? > c1
make clean
$CDIR/reference-implementations/A1-sclp $CDIR/example-programs/Level-$1-invalid-test-cases/l$1-invalid-exmp$2.c --show-tokens -d > t2
echo $? > c2
clear
# diff -Bw t1 t2
echo "----"
diff -Bw c1 c2
cat c1
rm -f t1 t2 c1 c2