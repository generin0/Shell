#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <time.h>
#include <sys/stat.h>

#define MAX_ARGS 10
/* colors */
#define GREEN "\x1b[32m"
#define RED "\x1b[31m"
#define RESET "\x1b[0m"
#define CYAN "\x1b[36m"

void exit_command() {
    printf(RED"Closing the shell.\n"RESET);
    exit(0);
}

void whoami_command(char **args) {
  char *user = getenv("USER");
  if (user) {
    printf(GREEN"%s\n"RESET, user);
  } else {
    printf(RED"Cannot find the current user.\n"RESET);
  }
}

void cat_command(char **args) {           // needs to be finished (cannot write)
  FILE *file = fopen(args[1], "r");
  if (file == NULL) {
    printf("Cannot open the %s file.\n", args[1]);
    return;
  }
  int ch;
  while ((ch = fgetc(file)) != EOF) {
    putchar(ch);
  }
  fclose(file);
}

void mkdir_command(char **args) {
  if (args[1] == NULL) {
    printf("Usage: mkdir <directory_name>\n");
    return;
  }
  if (mkdir(args[1], 0755) == 0) {
    printf(GREEN"Directory %s successfully created.\n"RESET, args[1]);
  } else {
    perror(RED"mkdir"RESET);
  }
}

void rmdir_command(char **args) {
  if (args[1] == NULL) {
    printf("Usage: rmdir <directory_name>\n");
    return;
  }
  if (rmdir(args[1]) == 0) {
    printf(GREEN"Successfully deleted %s directory.\n"RESET, args[1]);
  } else {
    perror(RED"Cannot delete directory"RESET);
  }
}

void mytime_command() {
    time_t current_time = time(NULL);
    if (current_time == -1){
        perror(RED"Failed"RESET);
    } else {
        struct tm *tm = localtime(&current_time);
        if (tm == NULL){
            perror(RED"Localtime failed"RESET); 
        } else {
            char time_str[100];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);
            printf("%s\n", time_str);
        }
    }
}

void echo_command(char **args) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
}

void clear_command() {
    printf("\033[2J\033[H");
}

void cd_command(char **args) {
    if (args[1] == NULL) {
        chdir(getenv("HOME"));
    } else {
        if (chdir(args[1])) {
            perror(RED"Cd failed"RESET);
        }
    }
}

void help_command() {
    printf("\n");
    printf(GREEN"═══ SHELL COMMANDS ═══\n"RESET);
    printf(CYAN"Built-in commands:\n"RESET);
    printf("  help       - Show this help\n");
    printf("  exit       - Exit the shell\n");
    printf("  cd <dir>   - Change directory\n");
    printf("  pwd        - Print working directory\n");
    printf("  clear      - Clear screen\n");
    printf("  mytime     - Show current date and time\n");
    printf("  echo [text]- Print text (supports -n flag)\n");
    printf("  mkdir <dir>- Create directory\n");
    printf("  rmdir <dir>- Remove empty directory\n");
    printf("  cat <dir>  - Display content of file\n");
    printf("  whoami     - Shows the current user\n");
    printf(GREEN"External commands are also supported!\n"RESET);
    printf("\n");
}

void pwd_command() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        printf("%s\n", cwd);
    } else {
        perror(RED"Pwd failed"RESET);
    }
}

void execute_external(char **args) {
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

void execute_command(char **args) {
    if (strcmp(args[0], "exit") == 0) {
        exit_command();
    } else if (strcmp(args[0], "mytime") == 0) {
        mytime_command(args);
    } else if (strcmp(args[0], "echo") == 0) {
        echo_command(args);
    } else if (strcmp(args[0], "clear") == 0) {
        clear_command();
    } else if (strcmp(args[0], "cd") == 0) {
        cd_command(args);
    } else if (strcmp(args[0], "help") == 0) {
        help_command();
    } else if (strcmp(args[0], "pwd") == 0) {
        pwd_command(args);
    } else if (strcmp(args[0], "mkdir") == 0) {
        mkdir_command(args);
    } else if (strcmp(args[0], "rmdir") == 0) {
        rmdir_command(args);
    } else if (strcmp(args[0], "cat") == 0) {
        cat_command(args);
    } else if (strcmp(args[0], "whoami") == 0) {
        whoami_command(args);
    } else {
        execute_external(args);
    }
}

int main() {
    printf("\n");
    printf(GREEN"═══ SHELL COMMANDS ═══\n"RESET);
    printf(CYAN"Built-in commands:\n"RESET);
    printf("  help       - Show this help\n");
    printf("  exit       - Exit the shell\n");
    printf("  cd <dir>   - Change directory\n");
    printf("  pwd        - Print working directory\n");
    printf("  clear      - Clear screen\n");
    printf("  mytime     - Show current date and time\n");
    printf("  echo [text]- Print text (supports -n flag)\n");
    printf("  mkdir <dir>- Create directory\n");
    printf("  rmdir <dir>- Remove empty directory\n");
    printf("  cat <dir>  - Display content of file\n");
    printf("  whoami     - Shows the current user\n");
    printf(GREEN"External commands are also supported!\n"RESET);
    printf("\n");

    while(1) {
        char *input = readline("Shell>");
        if (!input) {
            printf("\n");
            break;
        }

        if (strlen(input) > 0) {
            add_history(input);
        }

        if (strlen(input) == 0) {
            free(input);
            continue;
        }

        char *start = input;
        while(*start == ' ' || *start == '\t') *start++;
        if (*start == '\0') continue;

        char *args[MAX_ARGS];
        int argc = 0;
        char *word = strtok(input, " ");

        while (word != NULL && argc < MAX_ARGS - 1) {
            args[argc] = word;
            argc++;
            word = strtok(NULL, " ");
        }

        args[argc] = NULL;

        if (argc > 0) {
            execute_command(args);
        }
    }
    return 0;
}
