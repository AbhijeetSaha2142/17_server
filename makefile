all: prompt.c process.c
	gcc -o prompt prompt.c
	gcc -o process process.c
run:
	./process &
	./prompt