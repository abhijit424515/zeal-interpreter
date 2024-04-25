./reference-implementations/A4-sclp test.c -d --show-tac > y2
./reference-implementations/A4-sclp test.c -d --show-rtl --suppress-comments > z2

cd ../../work/A4/
make > /dev/null
cd ../../ps/A4-resources/

../../work/A4/sclp test.c -d --show-tac > y1    
../../work/A4/sclp test.c -d --show-rtl --suppress-comments > z1

# clear

diff -Bw z1 z2