#include "shell.h"

/* msh_echo: prints to screen a list of strings. */
int msh_echo(char **args, int count) {
  for (int i = 0; i < count && *(args + i); i++)
    printf((i == count - 1) ? "%s" : "%s ", *(args + i));
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
void msh_pwd() {}
