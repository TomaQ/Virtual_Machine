compiler: main.o vm.h lex.h
	gcc -o compiler main.o

main.o: main.c
	gcc -c main.c

v:
	./compiler -v

l:
	./compiler -l

clean:
	rm main.o
	rm compiler

