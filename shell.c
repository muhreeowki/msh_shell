// TODO: Implement the following features.
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
#include <unistd.h>

int main(int argc, char *argv[]) {
  repl();

  return 0;
}

char *io_redirect_perm = "w", *err_redirect_perm = "w",
     *io_redirect_filename = "", *err_redirect_filename = "";

void repl() {
  char **tokens = NULL, *line = NULL;
  int total_tokens, status, stdout_backup_fd, out_file_fd, stderr_backup_fd,
      err_file_fd;
  FILE *out_file, *err_file; // redirect files

  // Read Eval Print Loop
  for (;;) {
    total_tokens = 0;
    io_redirect_perm = io_redirect_filename = err_redirect_perm =
        err_redirect_filename = "";

    // set output default output files.
    out_file = err_file = NULL;
    stdout_backup_fd = dup(1);
    stderr_backup_fd = dup(2);

    /* Print the prompt*/
    printf("$ ");

    /* Read input (getline). Exit on EOF. */
    line = read_line();
    // printf("line: %s\n", line);
    if (!line) {
      printf("bye.\n");
      exit(EXIT_FAILURE);
    }

    /* Scanning/Lexing */
    tokens = get_tokens(line, &total_tokens);
    if (tokens == NULL)
      continue;

    // print_tokens(tokens);

    /* Redirecting output */
    if (strlen(io_redirect_perm) > 0) {
      if (strlen(io_redirect_filename) > 0) {
        // Open the output file
        out_file = fopen(io_redirect_filename, io_redirect_perm);
        if (out_file == NULL) {
          perror("msh");
          continue;
        }
        // Get the file descriptor
        out_file_fd = out_file->_fileno;
        // Replace stdout with output file
        dup2(out_file_fd, 1);
        // close output file
        close(out_file_fd);
      } else {
        printf("msh: parse error: no redirect filename provided");
      }
    }

    /* Redirecting error output */
    if (strlen(err_redirect_perm) > 0) {
      if (strlen(err_redirect_filename) > 0) {
        // Open the output file
        err_file = fopen(err_redirect_filename, err_redirect_perm);
        if (err_file == NULL) {
          perror("msh");
          continue;
        }
        // Get the file descriptor
        err_file_fd = err_file->_fileno;
        // Replace stdout with output file
        dup2(err_file_fd, 2);
        // close output file
        close(err_file_fd);
      } else {
        printf("msh: parse error: no redirect filename provided");
      }
    }

    /* Run commands */
    status = executor(tokens);

    // Clean the redirected output file and restore to stdout
    if (out_file != NULL) {
      dup2(stdout_backup_fd, 1);
      close(stdout_backup_fd);
      fclose(out_file);
    }

    // Clean the redirect error output file and restore to stderr
    if (err_file != NULL) {
      dup2(stderr_backup_fd, 2);
      close(stderr_backup_fd);
      fclose(err_file);
    }

    /* clean allocated memory */
    for (int i = 0; *(tokens + i) != NULL; i++)
      free(*(tokens + i));
    free(tokens);
    free(line);
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
      // printf("reallocated memory!");
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
char **get_tokens(char *line, int *total_tokens) {
  char *buf, **tokens;
  State state = NORMAL, prev_state = NORMAL;
  IOState io_state = DEFAULT;
  int c = 0, i = 0, list_size = TOKENS_LIST_SIZE, token_count = 0,
      buf_size = BUFSIZE, buf_count = 0;

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
    if (token_count >= list_size - 2) {
      list_size += TOKENS_LIST_SIZE;
      tokens = realloc(tokens, list_size);
      if (tokens == NULL) {
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
    // TODO: Make good use of the fall through feature
    switch (c) {

    case ' ':
      if (state != NORMAL) {
        *(buf + buf_count++) = c;
        *(buf + buf_count) = '\0';
        if (state == ESCAPED)
          state = prev_state;
      } else {
        // Skip trailing whitespace
        if (*buf == '\0') {
          continue;
        }

        // Emit the token
        char *token = malloc(sizeof(char) * (buf_count + 1));
        token = strncpy(token, buf, buf_count + 1);
        // Reset the Buffer
        *buf = '\0';
        buf_count = 0;
        // Set the redirect output
        if (io_state == REDIRECT_STDOUT) {
          io_redirect_filename = token;
          io_state = DEFAULT;
        } else if (io_state == REDIRECT_STDERR) {
          err_redirect_filename = token;
          io_state = DEFAULT;
        } else {
          *(tokens + token_count++) = token;
        }
      }
      break;

    case '\'':
      if (state == NORMAL) {
        state = IN_SQUOTES;
      } else if (state == IN_SQUOTES) {
        state = NORMAL;
      } else {
        if (state == ESCAPED) {
          if (prev_state == IN_DQUOTES)
            *(buf + buf_count++) = '\\';
          state = prev_state;
        }
        *(buf + buf_count++) = c;
        *(buf + buf_count) = '\0';
      }
      break;

    case '"':
      if (state == NORMAL) {
        state = IN_DQUOTES;
      } else if (state == IN_DQUOTES) {
        state = NORMAL;
      } else {
        *(buf + buf_count++) = c;
        *(buf + buf_count) = '\0';
        if (state == ESCAPED) {
          state = prev_state;
        }
      }
      break;

    case '\\':
      if (state == NORMAL || state == IN_DQUOTES) {
        prev_state = state;
        state = ESCAPED;
      } else {
        *(buf + buf_count++) = c;
        *(buf + buf_count) = '\0';
        if (state == ESCAPED)
          state = prev_state;
      }
      break;

    case '>':
      if (state == NORMAL) {
        char *perm = "w";
        // Check the next char
        if (*(line + i) == '>') {
          perm = "a+";
          i++;
        }
        if (buf_count == 0 || (buf_count == 1 && *buf == '1')) {
          // Redirect stdout
          io_state = REDIRECT_STDOUT;
          io_redirect_perm = perm;
        } else if (buf_count == 1 && *buf == '2') {
          // Redirect stderr
          io_state = REDIRECT_STDERR;
          err_redirect_perm = perm;
        } else {
          // Emit the previous token
          char *token = malloc(sizeof(char) * (buf_count + 1));
          token = strncpy(token, buf, buf_count + 1);
          *(tokens + token_count++) = token;
        }
        // Reset the Buffer
        *buf = '\0';
        buf_count = 0;
        break;
      }
      // Fall through if not in NORMAL state

    default:
      // TODO: Handle Escaped Chars
      if (state == ESCAPED) {
        // if (c == 'n')
        //   c = '\n';
        // if (c == 't')
        //   c = '\t';
        if (prev_state == IN_DQUOTES)
          *(buf + buf_count++) = '\\';
        state = prev_state;
      }
      *(buf + buf_count++) = c;
      *(buf + buf_count) = '\0';
      break;
    }
  }

  // Emit last token
  if (*buf != '\0') {
    char *token = malloc(sizeof(char) * (buf_count + 1));
    token = strncpy(token, buf, buf_count + 1);
    // Set the redirect output
    if (io_state == REDIRECT_STDOUT) {
      io_redirect_filename = token;
      io_state = DEFAULT;
    } else if (io_state == REDIRECT_STDERR) {
      err_redirect_filename = token;
      io_state = DEFAULT;
    } else {
      *(tokens + token_count++) = token;
    }
  }

  // Null terminate the tokens list
  *(tokens + token_count++) = NULL;

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

  // print_tokens(tokens, token_count);
  // free(buf);
  *total_tokens = token_count;
  return tokens;
}

int executor(char **tokens) {
  char *cmd = *tokens;

  // Check if the command is a builtin
  // - handle pwd
  if (strcmp(cmd, "pwd") == 0)
    return msh_pwd(NULL);
  // - handle exit
  if (strcmp(cmd, "exit") == 0)
    msh_exit(0);
  // - handle other builtins
  for (int i = 0; i < (sizeof builtins / sizeof(Builtin)); i++) {
    if (strcmp(cmd, builtins[i].name) == 0)
      return builtins[i].func(++tokens);
  }

  // Not a builtin, run the binary from the system
  return run_program(tokens);
}

/*
 * run_program: runs program files in the system.
 * It returns the exit status of the program.
 */
int run_program(char **tokens) {
  // TODO: Build this from scratch
  char *cmd = *tokens;
  pid_t pid, wpid;
  int status;

  pid = fork();

  if (pid == 0) { // Child Process
    // Execute the command
    execvp(cmd, tokens);
    // If the exec function returns, there was an error.
    printf("%s: command not found\n", cmd);
    // perror("invalid command");
    exit(EXIT_FAILURE);
  } else if (pid == -1) { // Error forking the process
    perror("msh");
  } else { // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return status;
}

/* print_tokens: prints a list of tokens. */
void print_tokens(char **tokens) {
  for (int i = 0; *(tokens + i) != NULL; i++)
    printf("[index: %d] [len: %lu] %s\n", i, strlen(*(tokens + i)),
           *(tokens + i));
}
