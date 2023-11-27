CC=gcc
CFLAGS=-Wall
DEPS= lib/last_output.h lib/cd.h lib/exit.h lib/prompt.h lib/pwd.h lib/parseur.h lib/env.h
OBJ = cmd/last_output.o cmd/cd.o cmd/exit.o cmd/prompt.o cmd/pwd.o parseur.o 
EXEC=jsh

all : $(EXEC)

%.o: %.c $(DEPS)
		$(CC) $(CFLAGS) -o $@ -c $< -lreadline

jsh : main.o $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^ -lreadline

test : tests/test.o $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^ -lreadline

clean :
		rm -rf $(EXEC) *.o
		rm -rf $(EXEC) *.exe
		rm -rf $(EXEC) test
		rm -rf $(EXEC) */*.o