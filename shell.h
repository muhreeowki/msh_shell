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

#define TOKENS_BUFSIZE 64
#define BUFSIZE 1024

void repl();
char *read_line();
int count_tokens(const char *line);
char **get_tokens(char *line);
void print_tokens(char **tokens, int lim);
int run_program(char **tokens);

#endif // !SHELL_H
