all: client.c server.c
	gcc -o client client.c
	gcc -o server server.c
run:
	./server &
	./client