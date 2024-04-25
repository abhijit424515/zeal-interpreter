CPP = g++
BISON = yacc
FLEX = lex
TGT = sclp
SCAN = $(TGT).l
PARSE = $(TGT).y

OBJ = scan.o y.tab.o main.o

CFLAGS = -g

$(TGT): $(OBJ)
	$(CPP) --std=c++0x  $(OBJ) -o $(TGT) -ly 

scan.o: scan.c $(HEADERS)
	$(CPP) --std=c++0x  $(CFLAGS) -c $<

y.tab.o:y.tab.c $(HEADERS)
	$(CPP) --std=c++0x  $(CFLAGS) -c  $<

%.o: %.cc $(HEADERS)
	$(CPP) --std=c++0x  $(CFLAGS) -c $<

scan.c : $(SCAN) y.tab.h
	$(FLEX) -l --yylineno -o scan.c $(SCAN)

y.tab.c y.tab.h : $(PARSE)
	$(BISON) -dv  $(PARSE)

clean :
	rm -f *.o *.output *.log *.toks
	rm -f $(TGT) 
	rm -rf *.tab.c *.tab.h scan.c 
