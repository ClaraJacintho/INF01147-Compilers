all: etapa4

etapa4: ast.o errors.o symbol_table.o parser.tab.c lex.yy.c 
	gcc -c main.c lex.yy.c 
	gcc -c parser.tab.c
	gcc -o etapa4 main.o ast.o lex.yy.o parser.tab.o symbol_table.o errors.o -lfl -ggdb3 -Wall

parser.tab.c: parser.y
	bison -d parser.y

lex.yy.c: scanner.l
	flex scanner.l

ast.o:
	gcc -c ast.c -o ast.o  -ggdb3 -Wall

symbol_table.o: 
	gcc -c symbol_table.c -o symbol_table.o  -ggdb3 -Wall

errors.o:
	gcc - errors.c -o errors.o  -ggdb3 -Wall

test:
	./etapa4 < test_a

debug:
	valgrind ./etapa4 < test_a  --leak-check=full
clean:
	rm -f lex.yy.* main.o ast.o symbol_table.o parser.tab.* parser.output etapa4

