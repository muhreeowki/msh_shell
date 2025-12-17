#include "shell.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* msh_echo: prints to screen a list of strings. */
int msh_echo(char **args) {
  while (*args != NULL) {
    printf(*(args + 1) != NULL ? "%s " : "%s", *args);
    args++;
  }
  putchar('\n');
  return 0;
}

/* msh_exit: like exit, exits the shell with the provided status.*/
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
  printf("%s\n", buf);
  free(buf);
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

  char *home_dir = _getenv("HOME");
  if (*args) {
    // Handle home character
    if (strcmp("~", *args) == 0) {
      status = chdir(home_dir);
    } else { // Run normal cd command
      status = chdir(*args);
    }
  } else {
    status = chdir(home_dir);
  }

  if (status != 0)
    printf("cd: %s: No such file or directory\n", *args);
  // print new directory
  // printf("msh_chdir: %s\n", getcwd(buf, LINE_BUFSIZE));
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
  int count = 1, i;
  char *original_environ, **paths, *path, *env;

  // check if environ variable exists.
  original_environ = _getenv("PATH");
  if (!original_environ)
    return NULL;
  // make a copy of the environ variable so you dont modifiy the entire environ
  env = malloc(sizeof(char) * strlen(original_environ) + 1);
  env = strcpy(env, original_environ);
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
  // create the paths list
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
  int i = 0;
  // Check if command is a builtin
  for (int i = 0; i < (sizeof(builtins) / sizeof(Builtin)); i++) {
    if (strcmp(cmd, builtins[i].name) == 0) {
      printf("%s is a shell builtin\n", cmd);
      return 0;
    }
  }
  // Search system for file
  char **paths, *path;
  struct stat sb;

  path = malloc(LINE_BUFSIZE);
  paths = getpaths();

  for (i = 0; paths[i] != NULL; i++) {
    sprintf(path, "%s/%s", paths[i], cmd);
    if (stat(path, &sb) == 0) {
      if (sb.st_mode & S_IXUSR) {
        printf("%s is %s\n", cmd, path);
        return 0;
      }
    }
  }

  printf("%s: not found\n", cmd);
  // free(path);
  // free(*paths);
  // free(paths);
  return 1;
}
