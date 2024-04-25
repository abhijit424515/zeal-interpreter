CFILE="../../ps/A2-resources/example-programs/1.c"

./sclp $CFILE > ../../ps/A2-resources/example-programs/z1
../../ps/A2-resources/reference-implementations/A2-sclp $CFILE -d --show-ast > ../../ps/A2-resources/example-programs/z2
diff -Bw ../../ps/A2-resources/example-programs/z1 ../../ps/A2-resources/example-programs/z2