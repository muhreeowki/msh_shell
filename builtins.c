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
void msh_exit(int *status) {
  if (status)
    exit(*status);
  exit(EXIT_SUCCESS);
}

/* msh_pwd: like pwd, prints currect working directory. */
int msh_pwd() {
  char *buf = malloc(sizeof(char) * BUFSIZE);

  printf("msh_pwd: %s\n", getcwd(buf, BUFSIZE));
  return 0;
}

/* msh_help: print the help page for a specific. */
int msh_help(char **args) {
  printf("msh_help \n");
  return 0;
}

/* msh_chdir: bulitin cd command. */
int msh_chdir(char **args) {
  int status = chdir(*args);
  char *buf = malloc(sizeof(char) * BUFSIZE);

  printf("msh_chdir: %s\n", getcwd(buf, BUFSIZE));
  return status;
}

char *_getenv(char *name) {
  char **env = __environ, *np;

  while (*env) {
    np = name;
    while (*np && *(*env)++ == *np)
      np++;

    if (*np == '\0')
      return ++(*env);
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
