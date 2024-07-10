all:
	gcc -I ./include -c ./code/fibo.c -o ./code/fibo.o
	gcc -I ./include -c ./code/main.c -o ./code/main.o
	gcc ./code/fibo.o ./code/main.o -o fibo
clean:
	rm ./code/fibo.o ./code/main.o
