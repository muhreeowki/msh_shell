#ifndef SHELL_H
#define SHELL_H

#include <ctype.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

// TODO: Use Unions or Something that allows for multiple
// function declarations to be used in a single struct.
typedef int (*BulitinFunc)(char **);

typedef struct builtin {
  char *name;
  BulitinFunc func;
} Builtin;

#define TOKENS_BUFSIZE 64
#define BUFSIZE 1024

// SHELL REPL FUNCTIONS
void repl();
char *read_line();
char **get_tokens(char *line);
int run_program(char **tokens);
void print_tokens(char **tokens, int lim);
int executor(char **tokens);

// BUILTIN FUNCTIONS
int msh_echo(char **args);
int msh_help(char **args);
int msh_chdir(char **args);

void msh_exit(int *status);
int msh_pwd();

static Builtin builtins[] = {
    {"echo", msh_echo}, {"help", msh_help}, {"cd", msh_chdir}};

#endif // !SHELL_H
