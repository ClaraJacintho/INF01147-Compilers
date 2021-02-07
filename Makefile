
all: lex.yy.c
	gcc main.c lex.yy.c -o etapa1 -lfl

lex.yy.c: scanner.l
	flex scanner.l

test:
	./etapa1 <tests/test_a
	./etapa1 <tests/test_b
	./etapa1 <tests/test_c
	./etapa1 <tests/test_d

clean:
	rm lex.yy.c etapa1
