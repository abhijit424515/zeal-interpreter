CPP = g++
BISON = bison
FLEX = flex
TGT = sclp
SCAN = $(TGT).l
PARSE = $(TGT).y

OBJ = scan.o parse.tab.o main.o support.o 

CFLAGS = -g -fpermissive

HEADERS = common-headers.hh

$(TGT): $(OBJ)
	$(CPP) --std=c++0x  $(OBJ) -o $(TGT) -ly 

scan.o: scan.c $(HEADERS)
	$(CPP) --std=c++0x  $(CFLAGS) -c $<

parse.tab.o: parse.tab.c $(HEADERS)
	$(CPP) --std=c++0x  $(CFLAGS)  -Wno-write-strings -c  $<

%.o: %.cc $(HEADERS)
	$(CPP) --std=c++0x  $(CFLAGS) -c $<

scan.c : $(SCAN) parse.tab.h
	$(FLEX) -l --yylineno -o scan.c $(SCAN)

parse.tab.c parse.tab.h : $(PARSE)
	$(BISON) -b parse -dv  $(PARSE)

clean :
	rm -f *.o *.output *.log *.toks
	rm -f $(TGT) 
	rm -rf *.tab.c *.tab.h scan.c 
