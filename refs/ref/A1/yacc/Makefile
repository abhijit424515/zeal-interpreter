PARSE = ex1
SCAN = ex

$(PARSE) : lex.yy.o y.tab.o
	gcc -o $(PARSE) lex.yy.o y.tab.o -ly -ll

y.tab.o y.tab.h : $(PARSE).y
	yacc -dv $(PARSE).y
	gcc -c y.tab.c

lex.yy.o : $(SCAN).l y.tab.h
	lex $(SCAN).l
	gcc -c lex.yy.c

clean :
	rm *.o lex.yy.* *.tab.c *.tab.h y.output exp? a.out $(PARSE)