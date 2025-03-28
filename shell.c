// TODO: Implement a POSIX shell with builtin functions like cut, cat, ls, cd,
// and more. It should have piping comands into one another, and writing output
// to files.

#include <ctype.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKENS_BUFSIZE 64
#define BUFSIZE 1024

char *read_line();
int count_tokens(const char *line);
char **get_tokens(char *line);
void print_tokens(char **tokens, int lim);

int main(int argc, char *argv[]) {
  // TODO: Handle bash files
  // TODO: Handle History
  // TODO: Handle builtin comands: ls, cd, cat, echo, help.
  // TODO: Handle advanced builtin comands: grep, find, cut, cp, mv.

  char **tokens = NULL, *line = NULL;
  int total_tokens;

  // Read Eval Print Loop
  for (;;) {
    printf("> ");
    /* Read input (getline). Exit on EOF. */
    line = read_line();
    if (!line) {
      printf("bye.");
      exit(EXIT_FAILURE);
    }
    /* Scanning/Lexing */
    tokens = get_tokens(line);
    if (!tokens)
      continue;
    /* Run commands */
    // Repeat
    free(line);
    free(tokens);
  }
  return 0;
}

/* _getline: gets line input of max length, stores it in line and returns the
 * length of the line. */
char *read_line() {
  int c, i = 0, max = BUFSIZE;
  char *line = malloc(sizeof(char) * max);

  if (!line) {
    printf("msh: failed to allocate memory for tokens.");
    exit(EXIT_FAILURE);
  }

  while ((c = getchar()) != EOF && c != '\n') {
    // reallocate memory as needed
    if (i >= max) {
      max += BUFSIZE;
      line = realloc(line, max);
      printf("reallocated memory!");
      if (!line) {
        printf("msh: failed to allocate memory for tokens.");
        exit(EXIT_FAILURE);
      }
    }
    *(line + i++) = c;
  }
  *(line + i) = '\0';

  if (c == EOF) {
    free(line);
    return NULL;
  }
  return line;
}

/* scan_tokens: splits an input line into a list of n tokens and returns the
 * number of tokens. */
// TODO: Edgecase -> hello'world' is one token but should be two
char **get_tokens(char *line) {
  int i = 0, in_double_quotes = 0, in_single_quotes = 0, size = TOKENS_BUFSIZE;
  char *token, *start = line, **tokens = malloc(size);

  while (*line) {
    // reallocate space as needed
    if (i >= size) {
      size += TOKENS_BUFSIZE;
      tokens = realloc(tokens, size);
      if (!tokens) {
        printf("msh: failed to allocate memory for tokens.");
        exit(EXIT_FAILURE);
      }
    }

    if (isspace(*start)) {
      start = ++line;
    } else if (*line == '"' && !in_single_quotes) {
      in_double_quotes = in_double_quotes ? 0 : 1;
      line++;
    } else if (*line == '\'' && !in_double_quotes) {
      in_single_quotes = in_single_quotes ? 0 : 1;
      line++;
    } else if (isspace(*line) && !in_double_quotes && !in_single_quotes) {
      *line = '\0';
      *(tokens + i++) = start;
      while (isspace(*++line))
        ;
      if(!(*line)) {
        start = NULL;
        break;
      }
      start = line;
    } else {
      line++;
    }
  }

  if (start && !isspace(*start))
    *(tokens + i++) = start;

  if (in_double_quotes || in_single_quotes) { /*non terminated string*/
    printf("error: non terminated quote\n");
    return NULL;
  }

  // print_tokens(tokens, i);
  return tokens;
}

/* print_tokens: prints a list of tokens. */
void print_tokens(char **tokens, int lim) {
  for (int i = 0; i < lim; i++)
    printf("[index: %d] [len: %lu] %s\n", i, strlen(*(tokens + i)),
           *(tokens + i));
}
