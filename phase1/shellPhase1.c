#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <assert.h>
#include <sys/wait.h>

#define LINEBUFFSIZE 1024
#define TOKENBUFFSIZE 64
#define TOKENDELIM " \n\t\r\a"
#define SHELLESCAPE "quit"

#define NODEBUG

int hist_count = 0;
int j = 0;
struct history {
  int num;
  char *line;
} hist[25];

void nssh_loop();
char *read_cmdline();
char **split_line(char *);
int nssh_execute(char **);
void add_hist(char *);
void show_hist();
int getch(void);

int main() {
  nssh_loop();
  return 0;
}

void nssh_loop() {
  char *line;
  char **args;
  int status = 1, chg;

  do {

    /*while (getch() == 27) { // if the first value is esc ^[
       getch();              // skip the [
       switch (getch()) {    // the real value
       case 65:
        printf("upp\n");
        printf("nssh> ");
        // code for arrow up
        break;
       case 66:
        printf("dowwn\n");
        printf("nssh> ");
        // code for arrow down
        break;
       }
       }*/
    printf("nssh> ");
    line = read_cmdline();
    add_hist(line);
    args = split_line(line);
    if (args != '\0')
      status = nssh_execute(args);
  } while (status);

  return;
}

char *read_cmdline() {
  int buffsize = LINEBUFFSIZE;
  int position = 0;

  char *cmd_buff = malloc(sizeof(char) * buffsize);
  int ch, chg;

  if (!cmd_buff) {
    printf("Error allocating the buffer.\n");
    exit(1);
  }

  while (1) {
    ch = getchar();

    if (ch == EOF || ch == '\n') {
      cmd_buff[position] = '\0';

#ifdef DEBUG
      printf("cmdbuff: %s\n", cmd_buff);
#endif

      return cmd_buff;
    } else {
      cmd_buff[position] = ch;
    }
    position++;

    if (position >= buffsize) {
      buffsize += LINEBUFFSIZE;
      cmd_buff = realloc(cmd_buff, buffsize);
      if (!cmd_buff) {
        printf("Error Reallocating the buffer. \n");
        exit(1);
      }
    }
  }
}

char **split_line(char *line) {
  int tokbuff = TOKENBUFFSIZE;
  int position = 0;
  char **token_list = malloc(sizeof(char *) * tokbuff);
  char *token;

  if (!token_list) {
    printf("Error allocating Token buffer.\n");
    exit(1);
  }

  token = strtok(line, TOKENDELIM);
  while (token != NULL) {
    token_list[position] = token;
    position++;

    if (position >= tokbuff) {
      tokbuff += TOKENBUFFSIZE;
      token_list = realloc(token_list, tokbuff * sizeof(char *));
      if (!token_list) {
        printf("Error reallocating token buffer.\n");
        exit(1);
      }
    }
    token = strtok(NULL, TOKENDELIM);
  }
  token_list[position] = NULL;

#ifdef DEBUG
  printf("token_list of 0 and 1: %s , %s\n", token_list[0], token_list[1]);
#endif

  return token_list;
}

int nssh_execute(char **argus) {
  int status;
  if (strcmp(argus[0], SHELLESCAPE) == 0) {
#ifdef DEBUG
    printf("command entered: %s\n", argus[0]);
#endif
    exit(0);
  }
  if (strcmp(argus[0], "cd") == 0) {
    chdir(argus[1]);
    return 1;
  }
  pid_t pid = fork(), wpid;
  if (pid == 0) {
    if (strcmp(argus[0], "showhist") == 0) {
      show_hist();
      exit(0);
    }
    execvp(argus[0], argus);
  }
  wait(NULL);
  return 1;
}

void add_hist(char *line) {
  /*hist[hist_count % 12] = line;
  hist_count++;*/

  hist[hist_count % 12].num = j;
  hist[hist_count % 12].line = line;
  hist_count++;
  j++;
}

void show_hist() {

  for (int i = 0; i < (hist_count); i++) {
    printf("%d---> %s\n", hist[i].num, hist[i].line);
  }
}

int getch(void) {
  int c = 0;

  struct termios org_opts, new_opts;
  int res = 0;
  //-----  store old settings -----------
  res = tcgetattr(STDIN_FILENO, &org_opts);
  assert(res == 0);
  //---- set new terminal parms --------
  memcpy(&new_opts, &org_opts, sizeof(new_opts));
  new_opts.c_lflag &=
      ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
  c = getchar();
  //------  restore old settings ---------
  res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
  assert(res == 0);
  return (c);
}
