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
#include <stdio.h>
#include <stdlib.h>
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
      printf("bye.\n");
      exit(EXIT_FAILURE);
    }
    /* Scanning/Lexing */
    tokens = get_tokens(line);
    if (!tokens)
      continue;
    /* Run commands */
    // status = executor(tokens);
    // Repeat
    free(line);
    free(tokens);
  }
}

/* _getline: gets line input of max length, stores it in line and returns the
 * length of the line. */
char *read_line() {
  int c, i = 0, max = LINE_BUFSIZE;
  char *line = malloc(sizeof(char) * max);

  if (!line) {
    printf("msh: failed to allocate memory for tokens.");
    exit(EXIT_FAILURE);
  }

  while ((c = getchar()) != EOF && c != '\n') {
    // reallocate memory as needed
    if (i >= max) {
      max += LINE_BUFSIZE;
      line = realloc(line, max);
      printf("reallocated memory!");
      if (!line) {
        printf("msh: failed to reallocate memory for tokens.");
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
char **get_tokens(char *line) {
  int c = 0, i = 0, list_size = TOKENS_LIST_SIZE, token_count = 0,
      buf_size = BUFSIZE, buf_count = 0;
  char *buf, **tokens;
  State state = NORMAL;

  tokens = malloc(sizeof(char *) * list_size);
  if (!tokens) {
    perror("msh: could not allocate memory");
    exit(1);
  }

  buf = malloc(sizeof(char) * BUFSIZE);
  if (!buf) {
    perror("msh: could not allocate memory");
    exit(1);
  }

  while ((c = *(line + i++))) {
    // reallocate token list space as needed
    if (token_count >= list_size) {
      list_size += TOKENS_LIST_SIZE;
      tokens = realloc(tokens, list_size);
      if (!tokens) {
        printf("msh: failed to allocate memory for tokens.");
        exit(EXIT_FAILURE);
      }
    }
    // reallocate buffer space as needed
    if (buf_count + 1 >= buf_size) {
      buf_size += BUFSIZE;
      buf = realloc(buf, buf_size);
      if (!buf) {
        printf("msh: failed to allocate memory for tokens.");
        exit(EXIT_FAILURE);
      }
    }

    // Build the token
    // printf("char: %c\n", c);
    switch (c) {
    case ' ':
      // printf("space\n");
      if (state != NORMAL) {
        *(buf + buf_count++) = c;
        *(buf + buf_count) = '\0';
        if (state == ESCAPED)
          state = NORMAL;
      } else {
        // printf("normal state\n");
        // Skip trailing whitespace
        if (*buf == '\0') {
          // printf("skiping trailing whitespace\n");
          continue;
        }
        // Emit the token
        char *token = malloc(sizeof(char) * (buf_count + 1));
        token = strcpy(token, buf);
        *(tokens + token_count++) = token;
        // Reset the Buffer
        *buf = '\0';
        buf_count = 0;
      }
      break;
    case '\'':
      if (state == NORMAL) {
        state = IN_SQUOTES;
      } else if (state == IN_SQUOTES) {
        // Check for Concatination: Check if the next char is another quote
        if (*(line + i) == '\'') {
          ++i;
          continue;
        }
        // Emit the token
        char *token = malloc(sizeof(char) * (buf_count + 1));
        token = strcpy(token, buf);
        *(tokens + token_count++) = token;
        // Reset the Buffer
        *buf = '\0';
        buf_count = 0;
        state = NORMAL;
      } else {
        *(buf + buf_count++) = c;
        *(buf + buf_count) = '\0';
        if (state == ESCAPED)
          state = NORMAL;
      }
      break;
    case '"':
      if (state == NORMAL) {
        state = IN_DQUOTES;
      } else if (state == IN_DQUOTES) {
        // Check for Concatination: Check if the next char is another quote
        if (*(line + i) == '"') {
          ++i;
          continue;
        }
        // Emit the token
        char *token = malloc(sizeof(char) * (buf_count + 1));
        token = strcpy(token, buf);
        *(tokens + token_count++) = token;
        // Reset the Buffer
        *buf = '\0';
        buf_count = 0;
        state = NORMAL;
      } else {
        *(buf + buf_count++) = c;
        *(buf + buf_count) = '\0';
        if (state == ESCAPED)
          state = NORMAL;
      }
      break;
    case '\\':
      if (state == NORMAL) {
        state = ESCAPED;
      } else {
        *(buf + buf_count++) = c;
        *(buf + buf_count) = '\0';
        if (state == ESCAPED)
          state = NORMAL;
      }
      break;
    default:
      // TODO: Handle Escaped Chars
      if (state == ESCAPED) {
        state = NORMAL;
      }
      *(buf + buf_count++) = c;
      *(buf + buf_count) = '\0';
      break;
    }
  }
  // append last token
  if (*buf != '\0') {
    char *token = malloc(sizeof(char) * (buf_count + 1));
    token = strcpy(token, buf);
    *(tokens + token_count++) = token;
  }

  // Check for non terminated stuff
  switch (state) {
  case IN_SQUOTES:
    perror("msh: non terminated single quote");
    break;
  case IN_DQUOTES:
    perror("msh: non terminated double quote");
    break;
  default:
    break;
  }

  print_tokens(tokens, token_count);
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
