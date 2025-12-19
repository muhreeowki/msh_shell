#ifndef SHELL_H
#define SHELL_H

#include <ctype.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

// TODO: Use Unions or Something that allows for multiple
// function declarations to be used in a single struct.
typedef int (*BulitinFunc)(char **);
typedef enum { NORMAL, IN_SQUOTES, IN_DQUOTES, ESCAPED } State;
typedef enum {
  DEFAULT,
  REDIRECT_STDOUT,
  REDIRECT_STDERR,
  REDIRECT_A_STDOUT,
  REDIRECT_A_STDERR
} IOState;

typedef struct builtin {
  char *name;
  BulitinFunc func;
} Builtin;

#define TOKENS_LIST_SIZE 64
#define LINE_BUFSIZE 1024
#define BUFSIZE 16

// SHELL REPL FUNCTIONS
void repl();
char *read_line();
char **get_tokens(char *line, int *total_tokens);
int run_program(char **tokens);
void print_tokens(char **tokens);
int executor(char **tokens);

// BUILTIN FUNCTIONS
int msh_echo(char **args);
int msh_help(char **args);
int msh_chdir(char **args);
int msh_type(char **args);
int msh_pwd(char **args);
int msh_exit(int status);

// UTILS
char *_getenv(char *name);
char **getpaths();

// TODO: Impliment a hash table for the builtins so that you can find it by name
// instead of looping through to find it by index
static Builtin builtins[] = {{"echo", msh_echo}, {"help", msh_help},
                             {"cd", msh_chdir},  {"pwd", msh_pwd},
                             {"type", msh_type}, {"exit", NULL}};
#endif // !SHELL_H
