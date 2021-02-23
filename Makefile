
all: etapa2

etapa2: parser.tab.c lex.yy.c
	gcc -c lex.yy.c parser.tab.c
	gcc main.c -o etapa2 lex.yy.o parser.tab.o -lfl
parser.tab.c: parser.y
	bison -d parser.y

lex.yy.c: scanner.l
	flex scanner.l

test:
	./etapa2 < test_a
	./etapa2 < test_b
	./etapa2 < test_c
	./etapa2 < test_d
	./etapa2 < test_e

report: parser.y
	bison --report-file="r.txt" parser.y

clean:
	rm -f lex.yy.* main.o parser.tab.* parser.output etapa2

