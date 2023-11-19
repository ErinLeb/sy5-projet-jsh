CC=gcc
CFLAGS=-Wall
EXEC=jsh

all : $(EXEC)

jsh : cmd/last_output.o cmd/cd.o cmd/exit.o cmd/prompt.o cmd/pwd.o parseur.o main.o 
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 

clean :
	rm -rf $(EXEC) *.o
	rm -rf $(EXEC) *.exe
	rm -rf $(EXEC) */*.o
