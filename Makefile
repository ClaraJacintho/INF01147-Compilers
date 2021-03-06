
all: etapa3

etapa3: parser.tab.c lex.yy.c ast.o
	gcc -c lex.yy.c parser.tab.c
	gcc main.c -o etapa3 ast.o lex.yy.o parser.tab.o -lfl

parser.tab.c: parser.y
	bison -d parser.y

lex.yy.c: scanner.l
	flex scanner.l

ast.o:
	gcc -c ast.c -o ast.o -Wall

test:
	./etapa2 < test_a
	./etapa2 < test_b
	./etapa2 < test_c
	./etapa2 < test_d
	./etapa2 < test_e
	./etapa2 < test_spec

report: parser.y
	bison --report-file="r.txt" parser.y

clean:
	rm -f lex.yy.* main.o ast.o parser.tab.* parser.output etapa3

