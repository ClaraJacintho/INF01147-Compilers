all: etapa3

etapa3: ast.o parser.tab.c lex.yy.c 
	gcc -c main.c lex.yy.c 
	gcc -c parser.tab.c
	gcc -o etapa3 main.o ast.o lex.yy.o parser.tab.o -lfl -Wall

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

clean:
	rm -f lex.yy.* main.o ast.o parser.tab.* parser.output etapa3

