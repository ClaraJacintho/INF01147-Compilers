all: etapa6

etapa6: ast.o code_generation.o error_handling.o symbol_table.o parser.tab.c lex.yy.c assembly.o
	gcc -c main.c lex.yy.c 
	gcc -c parser.tab.c
	gcc -o etapa6 main.o ast.o lex.yy.o parser.tab.o symbol_table.o error_handling.o code_generation.o assembly.o -lfl -ggdb3 -Wall

parser.tab.c: parser.y
	bison -d parser.y

lex.yy.c: scanner.l
	flex scanner.l

assembly.o:
	gcc -c assembly.c -o assembly.o  -ggdb3 -Wall

ast.o:
	gcc -c ast.c -o ast.o  -ggdb3 -Wall

symbol_table.o: 
	gcc -c symbol_table.c -o symbol_table.o  -ggdb3 -Wall

error_handling.o:
	gcc -c error_handling.c -o error_handling.o  -ggdb3 -Wall

code_generation.o:
	gcc -c code_generation.c -o code_generation.o  -ggdb3 -Wall

test: etapa6
	./etapa6 < test_a

suber_debug: etapa6
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./etapa6 < test_a

debug: etapa6
	valgrind ./etapa6 < test_a	
	
clean:
	rm -f lex.yy.* main.o ast.o symbol_table.o assembly.o error_handling.o code_generation.o parser.tab.* parser.output etapa6

