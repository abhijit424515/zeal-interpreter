SOURCE="pic"
TARGET="runme"

run() {
	clear
	lex -l --yylineno -o scan.c $SOURCE.l && g++ --std=c++0x -g -c scan.c -o scan.o
	g++ --std=c++0x scan.o -o $TARGET

	./$TARGET
}

clean() {
  rm -f scan.c scan.o $TARGET
}

case $1 in
  run) run ;;
  clean) clean ;;
  *) echo "Usage: $0 {run|clean}" ;;
esac