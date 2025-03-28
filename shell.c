// TODO: Implement a POSIX shell with builtin functions like cut, cat, ls, cd,
// and more. It should have piping comands into one another, and writing output
// to files.

#include <ctype.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_line(char *line, int max);
int count_tokens(const char *line);
int scan_tokens(char *line, char **tokens, int n);
void print_tokens(char **tokens, int lim);

int main(int argc, char *argv[]) {
  // TODO: Handle bash files
  // TODO: Handle History
  // TODO: Handle builtin comands: ls, cd, cat, echo, help.
  // TODO: Handle advanced builtin comands: grep, find, cut, cp, mv.

  char **tokens, *line;
  int in_double_quotes = 0, in_single_quotes = 0;

  // Read Eval Print Loop
  for (;;) {
    line = malloc(sizeof(char) * ARG_MAX);
    printf("> ");
    /* Read input (getline). Exit on EOF. */
    if (read_line(line, ARG_MAX) == 0)
      return 0;
    /* Scanning/Lexing */
    // count
    int total_tokens = count_tokens(line);
    if (total_tokens == -1)
      continue;
    if (total_tokens > 0)
      printf("token count: %d\n", total_tokens);
    // allocate memory for list of tokens
    tokens = malloc(sizeof(char *) * total_tokens);
    // get all the tokens
    int total_scanned = scan_tokens(line, tokens, total_tokens);
    if (total_tokens != total_scanned) {
      printf("error: token count is %d but only scanned %d\n", total_tokens,
             total_scanned);
      continue;
    }
    print_tokens(tokens, total_scanned);
    // Run commands
    // Repeat
    free(line);
    free(tokens);
  }
  return 1;
}

/* _getline: gets line input of max length, stores it in line and returns the
 * length of the line. */
int read_line(char *line, int max) {
  int c, i = 0;

  while ((c = getchar()) != EOF && c != '\n' && i < max - 1)
    *(line + i++) = c;
  *(line + i) = '\0';
  return i;
}

/* count_tokens: returns the number of tokens in a line input. */
int count_tokens(const char *line) {
  int in_double_quotes = 0, in_single_quotes = 0, total_tokens = 1;
  const char *start = line;

  while (*line) {
    if (isspace(*start))
      start = ++line;
    else if (*line == '"' && !in_single_quotes)
      in_double_quotes = in_double_quotes ? 0 : 1, line++;
    else if (*line == '\'' && !in_double_quotes)
      in_single_quotes = in_single_quotes ? 0 : 1, line++;
    else if (isspace(*line) && !in_double_quotes && !in_single_quotes)
      start = line, total_tokens++;
    else
      line++;
  }

  if (in_double_quotes || in_single_quotes) { /*non terminated string*/
    printf("error: non terminated quote\n");
    return -1;
  }

  return total_tokens;
}

/* scan_tokens: splits an input line into a list of n tokens and returns the
 * number of tokens. */
// TODO: Edgecase -> hello'world' is one token but should be two
int scan_tokens(char *line, char **tokens, int lim) {
  int t = 0, in_double_quotes = 0, in_single_quotes = 0;
  char *start = line;

  while (*line) {
    if (isspace(*start)) {
      start = ++line;
    } else if (*line == '"' && !in_single_quotes) {
      in_double_quotes = in_double_quotes ? 0 : 1;
      line++;
    } else if (*line == '\'' && !in_double_quotes) {
      in_single_quotes = in_single_quotes ? 0 : 1;
      line++;
    } else if (isspace(*line) && !in_double_quotes && !in_single_quotes) {
      *line++ = '\0';
      *(tokens + t++) = start;
      start = line;
    } else {
      line++;
    }
  }

  *(tokens + t) = start;
  return t+1;
}

/* print_tokens: prints a list of tokens. */
void print_tokens(char **tokens, int lim) {
  for (int i = 0; i < lim; i++)
    printf("[index: %d] [len: %lu] %s\n", i, strlen(*(tokens + i)),
           *(tokens + i));
}
