all: etapa5

etapa5: ast.o code_generation.o error_handling.o symbol_table.o parser.tab.c lex.yy.c 
	gcc -c main.c lex.yy.c 
	gcc -c parser.tab.c
	gcc -o etapa5 main.o ast.o lex.yy.o parser.tab.o symbol_table.o error_handling.o  code_generation.o -lfl -ggdb3 -Wall

parser.tab.c: parser.y
	bison -d parser.y

lex.yy.c: scanner.l
	flex scanner.l

ast.o:
	gcc -c ast.c -o ast.o  -ggdb3 -Wall

symbol_table.o: 
	gcc -c symbol_table.c -o symbol_table.o  -ggdb3 -Wall

error_handling.o:
	gcc -c error_handling.c -o error_handling.o  -ggdb3 -Wall

code_generation.o:
	gcc -c code_generation.c -o code_generation.o  -ggdb3 -Wall

test: etapa5
	./etapa5 < test_a

suber_debug: etapa5
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./etapa5 < test_a

debug: etapa5
	valgrind ./etapa5 < test_a	
	
clean:
	rm -f lex.yy.* main.o ast.o symbol_table.o error_handling.o code_generation.o parser.tab.* parser.output etapa5

