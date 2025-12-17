#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* msh_echo: prints to screen a list of strings. */
int msh_echo(char **args) {
  while (*args) {
    printf(*(args + 1) ? "%s " : "%s", *args);
    args++;
  }
  putchar('\n');
  return 0;
}

/* msh_exit: like exit, exits the shell with the provided status. */
int msh_exit(int status) {
  if (status)
    exit(status);
  exit(EXIT_SUCCESS);
}

/* msh_pwd: like pwd, prints currect working directory. */
int msh_pwd(char **args) {
  char *buf = malloc(sizeof(char) * LINE_BUFSIZE);
  buf = getcwd(buf, LINE_BUFSIZE);
  if (!buf)
    perror("msh_pwd: couldn't get current directory.");
  printf("msh_pwd: %s\n", buf);
  return 0;
}

/* msh_help: print the help page for a specific. */
int msh_help(char **args) {
  printf("msh_help \n");
  return 0;
}

/* msh_chdir: bulitin cd command. */
int msh_chdir(char **args) {
  int status;
  char *buf = malloc(sizeof(char) * LINE_BUFSIZE);
  char *home_dir = _getenv("HOME");

  if (*args) {
    // Handle home character
    if (strcmp("~\n", *args)) {
      status = chdir(home_dir);
    } else { // Run normal cd command
      status = chdir(*args);
    }
  } else {
    status = chdir("");
  }
  // print new directory
  printf("msh_chdir: %s\n", getcwd(buf, LINE_BUFSIZE));
  return status;
}

char *_getenv(char *name) {
  char **env = __environ, *np, *v;

  while (*env) {
    np = name;
    v = *(env++);

    while (*np != '\0' && *v == *np) {
      np++;
      v++;
    }

    if (*np == '\0')
      return ++v;
  }

  return NULL;
}

/* getpaths: returns a list of strings of path names in the PATH environ
 * variable. */
char **getpaths() {
  char *env = _getenv("PATH"), **paths, *path;
  int count = 1, i;

  // check if environ variable exists.
  if (!env)
    return NULL;

  // count delims.
  for (i = 0; *(env + i); i++) {
    if (*(env + i) == ':')
      count++;
  }

  // allocate memory.
  paths = malloc(sizeof(char *) * ++count);
  if (!paths) {
    return NULL;
  }

  path = strtok(env, ":");
  for (i = 0; path != NULL && i < count; i++) {
    *(paths + i) = path;
    path = strtok(NULL, ":");
  }

  // NULL terminate the array;
  *(paths + i) = NULL;
  return paths;
}

/* msh_type: print the type of the command provided. */
int msh_type(char **args) {
  char *cmd = *args;
  for (int i = 0; i < (sizeof(builtins) / sizeof(Builtin)); i++) {
    if (strcmp(cmd, builtins[i].name) == 0) {
      printf("%s is a shell builtin\n", cmd);
      return 0;
    }
  }
  printf("invalid command: not found\n");
  return 1;
}
