/*
 *  Main file
 *  Written by Curtis Li
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>

#include "command.h"
#include "shell.h"
#include "parser.h"
#include "executer.h"
#include "debug.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/**************************************************
 *                                                *
 *    Functions                                   *
 *                                                *
 **************************************************/

/*
 *  Main function
 */
int main() {
  printf("Shell v0.1\n");
  printf("Created by Curtis Li\n");
  printf("--------------------\n");
  char user[1024], pn[1024], *line, *homedir, *p, *cwd = pn;
  Command *cmd;
  struct passwd *pw;

  if (getlogin_r(user, 1024) != 0) panic("getlogin_r");

  // Main program loop
  while(1) {
    // Prompt
    if ((homedir = getenv("HOME")) == NULL) {
      if ((pw = getpwuid(getuid())))
        homedir = pw->pw_dir;
    }
    if(!getcwd(pn, 1024)) panic("getcwd");
    if (strncmp(pn, homedir, strlen(homedir)) == 0) {
      cwd = pn + strlen(homedir) - 1;
      *cwd = '~';
    }
    printf(ANSI_COLOR_BLUE "%s:" ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "$ ", user, cwd);
    if (fflush(stdout) != 0) perror("fflush");

    // Read line
    line = read_line();
    cmd = parse_line(line);

#ifdef DEBUG
    Command *cur = cmd;
    while (cur) {
      int i;
      printf("argc = %d\n", cur->argc);
      for (i = 0; i < cur->argc; i++) {
        printf("argv[%d] = %s\n", i, cur->argv[i]);
      }
      for (i = 0; i < 3; i++) {
        printf("redirect[%d] = %s\n", i, cur->redirect[i]);
      }
      cur = cur->next;
    }
#endif

    // Execute
    exec_cmd(cmd, 0);

    // Free space
    free_cmds(cmd);
    free_line(line);
  }
}
