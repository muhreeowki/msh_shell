// TODO: Implement a POSIX shell with builtin functions like cut, cat, ls, cd,
// and more. It should have piping comands into one another, and writing output
// to files.

#include <ctype.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int _getline(char *line, int max);

int main(int argc, char *argv[]) {
  // TODO: Handle bash files
  // TODO: Handle History
  // TODO: Handle builtin comands: ls, cd, cat, echo, help.
  // TODO: Handle advanced builtin comands: grep, find, cut, cp, mv.

  char **tokens, *line = malloc(sizeof(char) * ARG_MAX);
  int in_double_quotes = 0, in_single_quotes = 0;

  // Read Eval Print Loop
  for (;;) {
    printf("> ");
    // Read input (getline). Exit on EOF.
    if (!_getline(line, ARG_MAX)) {
      printf("Bye.");
      return 0;
    };

    // Evaluate (Scanning/Lexing)
    int spaces = 0;
    char *lp, *start;
    for (lp = line; *lp; lp++) { /* count the spaces */
      if (*lp == '"' && !in_single_quotes)
        in_double_quotes = in_double_quotes ? 0 : 1;
      else if (*lp == '\'' && !in_double_quotes)
        in_single_quotes = in_single_quotes ? 0 : 1;
      else if (isspace(*lp) && !in_double_quotes && !in_single_quotes)
        spaces++;
    }

    tokens = malloc(sizeof(char *) * spaces + 1);
    // Split up the line into tokens
    lp = line, start = lp;
    int i, t;
    for (i = 0, t = 0; *(lp + i); i++) {
      if (*(lp + i) == '"' && !in_single_quotes)
        in_double_quotes = in_double_quotes ? 0 : 1;
      if (*(lp + i) == '\'' && !in_double_quotes)
        in_single_quotes = in_single_quotes ? 0 : 1;
      if (isspace(*(lp + i)) && !in_double_quotes && !in_single_quotes) {
        *(lp + i) = '\0';
        *(tokens + t++) = start;
        start = lp + i + 1;
      }
    }
    *(tokens + t) = start;
    // Run commands
    for (t = 0; t < spaces; t++)
      printf("[%d] %s\n", t, *(tokens + t));
    // Repeat
    free(tokens);
  }
  free(line);
  return 1;
}

int _getline(char *line, int max) {
  if (fgets(line, max, stdin) == NULL)
    return 0;
  else
    return strlen(line);
}
