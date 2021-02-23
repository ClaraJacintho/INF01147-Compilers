
all: etapa2

etapa2:	main.o
	gcc -o etapa2 main.o lex.yy.o parser.tab.o -lfl

main.o: parser.tab.c lex.yy.c
	gcc -c main.c lex.yy.c parser.tab.c

parser.tab.c: parser.y
	bison -d parser.y

lex.yy.c: scanner.l
	flex scanner.l

test:
	./etapa2 < test_a
	./etapa2 < test_b
	./etapa2 < test_c
	./etapa2 < test_d
report: parser.y
	bison --report-file="r.txt" parser.y

clean:
	rm lex.yy.* main.o parser.tab.* etapa2
