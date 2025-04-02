// TODO: Implement the following features.
// builtin comands: cd, cat, echo, help.
// environment variables: getenv, setenv, unsetenv, export etc.
// alliases.
// shell opperations: |, &, >, <, >>, <<, ||, &&
// script files.
// advanced builtin comands: fzf, grep, find, cut, cp, mv.
// tab completion.
// command history.
// reverse search command history.
// improved prompt.

#include "shell.h"
#include <string.h>

int main(int argc, char *argv[]) {
  repl();

  return 0;
}

void repl() {
  char **tokens = NULL, *line = NULL;
  int total_tokens, status;

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
    status = executor(tokens);
    // Repeat
    free(line);
    free(tokens);
  }
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
      if (!(*line)) {
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

int executor(char **tokens) {
  char *cmd = *tokens;

  if (strcmp(cmd, "pwd") == 0)
    return msh_pwd();

  for (int i = 0; i < (sizeof builtins / sizeof(Builtin)); i++) {
    if (strcmp(cmd, builtins[i].name) == 0)
      return builtins[i].func(++tokens);
  }

  return run_program(tokens);
}

/*
 * run_program: runs program files in the system.
 * It returns the exit status of the program.
 */
int run_program(char **tokens) {
  char *cmd = *tokens;
  pid_t pid, wpid;
  int status;

  pid = fork();

  if (pid == 0) { // Child Process
    // Execute the command
    // if the exec function returns, there was an error.
    if (!execvp(cmd, tokens)) {
      perror("msh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) { // Error forking the process
    perror("msh");
  } else { // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return status;
}

/* print_tokens: prints a list of tokens. */
void print_tokens(char **tokens, int lim) {
  for (int i = 0; i < lim; i++)
    printf("[index: %d] [len: %lu] %s\n", i, strlen(*(tokens + i)),
           *(tokens + i));
}
