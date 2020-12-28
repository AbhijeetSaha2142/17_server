all: prompt.c process.c
	gcc -o client client.c
	gcc -o server server.c
run:
	./server &
	./client