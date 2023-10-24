all: 	
	flex --outfile=scanner.c scanner.l
	bison -v --yacc --defines --output=parser.c parser.y
	gcc -g -o exe scanner.c parser.c 

run:
	./exe <test.txt

clean:
	rm -rf exe
	rm -rf parser.output
	rm -rf scanner.c
	rm -rf parser.c
	rm -rf parser.h

