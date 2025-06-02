#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <readline/history.h>
#include <readline/readline.h>

#define MAX_ARGS 10
/* colors */
#define RED "\x1b[31m"
#define RESET "\x1b[0m"
/*execute funcion*/
void execute(char **args) {
    if (strcmp(args[0], "exit") == 0) {
        printf("Closing the shell.\n");
        exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            chdir(getenv("HOME"));
        } else {
            if (chdir(args[1]) != 0) {
                perror(RED"Cd failed"RESET);
            }
        }
    } else if (strcmp(args[0], "help") == 0) {
        printf("Commands: cd, exit, pwd, help\n");
    } else if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd))) {
            printf("%s\n", cwd);
        } else {
            perror(RED"Pwd failed"RESET);
        }
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                perror(RED"Execvp failed"RESET);
                exit(1);
            }
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            perror(RED"Fork failed"RESET);
        }
    }
}

int main (){
  printf("Shell commands: cd, exit, pwd, help\n");

  while(1){
    char *input = readline("Shell>");
    
    if (!input){
      printf("\n");
      break;
    }

    if (strlen(input) > 0){
      add_history(input);
    }

    if (strlen(input) == 0){
      free(input);
      continue;
    }

    char *start = input;
    while(*start == ' ' || *start == '\t') *start++;
    if (*start == '\0') continue;
    
    char *args[MAX_ARGS];
    int argc = 0;
    char *word = strtok(input, " ");

    while (word != NULL && argc < MAX_ARGS - 1){
      args[argc] = word;
      argc++;
      word = strtok(NULL, " ");
    }

    args[argc] = NULL;

    if (argc > 0){
      execute(args);
    }
  }
return 0;
}
