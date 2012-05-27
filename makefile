CC=gcc
TPTLA: TPTLA.o
	gcc -o TPTLA TPTLA.o && flex Gramatica.l && gcc -o gramatica lex.yy.c -lfl && flex Automata.l && gcc -o automata lex.yy.c -lfl