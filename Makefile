FNAME = zeal
CPP = g++
BISON = bison
FLEX = flex
TGT = zeal

SCAN = $(FNAME).l
PARSE = $(FNAME).y
HEADERS = $(FNAME).hh

OBJ = scan.o parse.tab.o
CFLAGS = --std=c++14 -g

$(TGT): $(OBJ)
	$(CPP) $(CFLAGS) $(OBJ) -o $(TGT) -ly 

scan.o: scan.c $(HEADERS)
	$(CPP) $(CFLAGS) -c $<

parse.tab.o:parse.tab.c $(HEADERS)
	$(CPP) $(CFLAGS) -c  $<

%.o: %.cc $(HEADERS)
	$(CPP) $(CFLAGS) -c $<

scan.c : $(SCAN) parse.tab.h
	$(FLEX) -l --yylineno -o scan.c $(SCAN)

parse.tab.c parse.tab.h : $(PARSE)
	$(BISON) -b parse -dv $(PARSE) -Wcounterexamples

clean :
	rm -f *.o *.output
	rm -f $(TGT) 
	rm -rf parse.tab.c parse.tab.h scan.c 
