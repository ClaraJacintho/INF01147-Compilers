all: etapa3

etapa3: ast.o parser.tab.c lex.yy.c 
	gcc -c main.c lex.yy.c 
	gcc -c parser.tab.c
	gcc -o etapa3 main.o ast.o lex.yy.o parser.tab.o -lfl -ggdb3 -Wall

parser.tab.c: parser.y
	bison -d parser.y

lex.yy.c: scanner.l
	flex scanner.l

ast.o:
	gcc -c ast.c -o ast.o  -ggdb3 -Wall

test:
	./etapa3 < test_a
	./etapa3 < test_b
	./etapa3 < test_c
	./etapa3 < test_d
	./etapa3 < test_e
	./etapa3 < test_spec

clean:
	rm -f lex.yy.* main.o ast.o parser.tab.* parser.output etapa3

