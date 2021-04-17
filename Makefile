all: etapa4

etapa4: ast.o error_handling.o symbol_table.o parser.tab.c lex.yy.c 
	gcc -c main.c lex.yy.c 
	gcc -c parser.tab.c
	gcc -o etapa4 main.o ast.o lex.yy.o parser.tab.o symbol_table.o error_handling.o -lfl -ggdb3 -Wall

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

test: etapa4
	./etapa4 < test_x

debug: etapa4
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./etapa4 < test_a

nuclear_option: etapa4                               
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./etapa4 < test_a
	./nuclear_option.sh
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./etapa4 < test_spec
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./etapa4 < test_e3_errors
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./etapa4 < test_type_inference

clean:
	rm -f lex.yy.* main.o ast.o symbol_table.o error_handling.o parser.tab.* parser.output etapa4

