echo -n "" > y1
echo -n "" > y2
echo -n "" > z1
echo -n "" > z2
clear

./reference-implementations/A5-sclp test.c --show-rtl --sa-rtl --suppress-comments -d > y1
./reference-implementations/A5-sclp test.c --suppress-comments -d > z1

cd ../../work/A5/
make > /dev/null
cd ../../ps/A5-resources/

../../work/A5/sclp test.c -d --show-rtl --sa-rtl > y2
../../work/A5/sclp test.c -d > z2

diff -Bw y1 y2
diff -Bw z1 z2