cd work && make
./lp -c < ../custom_tests/$1 > ../z1
../lp -c < ../custom_tests/$1 > ../z2
diff -Bw ../z1 ../z2