CC = gcc
CFLAGS = -Wall -lreadline
DEPS = lib/last_output.h lib/cd.h lib/exit.h lib/prompt.h lib/pwd.h lib/parseur.h lib/commandes_externes.h lib/env.h lib/jobs.h lib/kill.h
OBJ = cmd/last_output.o cmd/cd.o cmd/exit.o cmd/prompt.o cmd/pwd.o cmd/commandes_externes.o parseur.o cmd/jobs.o cmd/kill.o
EXEC = jsh

all : $(EXEC)

%.o: %.c $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)

jsh : main.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

test : tests/test.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm -rf $(EXEC) *.o
	rm -rf $(EXEC) test
	rm -rf $(EXEC) jsh
	rm -rf $(EXEC) */*.o
