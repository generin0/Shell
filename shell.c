#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <readline/history.h>
#include <readline/readline.h>

#define RED "\x1b[31m"
#define RESET "\x1b[0m"

int main (){

  printf("Shell commands: cd, exit, pwd, help\n");

  while(1){
    char *args[10];
    char *input;

    input = readline("Shell>");
    
    if (!input){
      printf("\n");
      break;
    }

    if (strlen(input) > 0){
      add_history(input);
    }

    input[strcspn(input, "\n")] = 0;

    if (strlen(input) == 0){
      free(input);
      continue;
    }

    char *start = input;
    while(*start == ' ' || *start == '\t') *start++;
    if (*start == '\0') continue;

    int argc = 0;
    char *word = strtok(input, " ");
    while (word != NULL && argc < 9){
      args[argc] = word;
      argc++;
      word = strtok(NULL, " ");
    }

    args[argc] = NULL;

    if (strcmp(args[0], "exit") == 0){
      printf("Closing the shell.\n");
      break;
    }

    if (strcmp(args[0], "cd") == 0){
      if (argc > 1){
        if (chdir(args[1]) != 0){
          printf(RED "Syntax error: cannot find the directory: %s\n" RESET, args[1]);
        }
      }
      else{
        chdir(getenv("HOME"));
      }
      continue;
    }

    if (strcmp(args[0], "help") == 0){
      printf("Commands: cd, exit, pwd, help;\n");
      continue;
    }

    if (strcmp(args[0], "pwd") == 0){
      char cwd[1000];
      if (getcwd(cwd, sizeof(cwd)) != NULL){
        printf("%s\n", cwd);
        continue;
    }
    else{
      perror(RED "System error: cannot find the current directory path." RESET);
      }
    }

    pid_t pid = fork();

    if (pid == 0){
      if (execvp(args[0], args) == -1){
        printf(RED "Syntax error: cannot find the command: %s\n" RESET, args[0]);
        exit(1);
      }
    }
    else if (pid > 0){
      int status;
      waitpid(pid, &status, 0);
    }
    else{
      printf("System Error: cannot create the process.\n");
  }
}
return 0;
}
