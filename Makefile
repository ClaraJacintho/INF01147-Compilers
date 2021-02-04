
all: lex.yy.c
	gcc main.c lex.yy.c -o etapa1 -lfl

lex.yy.c: scanner.l
	flex scanner.l

test:
	./etapa1 <test_a

clean:
	rm lex.yy.c etapa1
