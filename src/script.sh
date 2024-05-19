SOURCE="pic"
TARGET="runme"
CFLAGS="--std=c++0x -g"

compile() {
	clear
	bison -b parse -dv pic.y															# generate parser using pic.y
	flex -l --yylineno -o scan.c $SOURCE.l								# generate scanner using pic.l 
	g++ $CFLAGS -c scan.c																	# compile scanner
	g++ $CFLAGS -Wno-write-strings -c parse.tab.c					# compile parser 
	g++ $CFLAGS scan.o parse.tab.o -o $TARGET -ly					# link the object files to create target
}

clean() {
  rm -f scan.c scan.o parse.tab.c parse.tab.h parse.output parse.tab.o $TARGET
}

case $1 in
  compile) compile ;;
	clean) clean ;;
  *) echo "Usage: $0 {compile|clean}" ;;
esac