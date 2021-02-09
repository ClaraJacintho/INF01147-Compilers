
all: etapa1

etapa1:	lex.yy.c
	gcc main.c lex.yy.c -o etapa1 -lfl

lex.yy.c: scanner.l
	flex scanner.l

test:
	./etapa1 <test_a
	./etapa1 <test_b
	./etapa1 <test_c
	./etapa1 <test_d

clean:
	rm lex.yy.c etapa1
