#include "shell.h"
#include <stdio.h>
#include <unistd.h>

/* msh_echo: prints to screen a list of strings. */
int msh_echo(char **args) {
  while (*args) {
    printf(*(args + 1) ? "%s ": "%s", *args);
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

int msh_help(char **args) { 
  printf("msh_help \n");
  return 0;
}

int msh_chdir(char **args) {
  int status = chdir(*args);
  char *buf = malloc(sizeof(char) * BUFSIZE);

  printf("msh_chdir: %s\n", getcwd(buf, BUFSIZE));
  return status; 
}
