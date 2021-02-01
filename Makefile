
all: lex.yy.c
	gcc main.c lex.yy.c -o etapa1 -lfl

lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm *.o lex.yy.c etapa1