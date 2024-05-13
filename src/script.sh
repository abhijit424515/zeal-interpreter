SOURCE="pic"
TARGET="runme"

run() {
  clear && flex $SOURCE.l && g++ lex.yy.c -o $TARGET && ./$TARGET
}

clean() {
  rm -f lex.yy.c $TARGET
}

case $1 in
  run) run ;;
  clean) clean ;;
  *) echo "Usage: $0 {run|clean}" ;;
esac