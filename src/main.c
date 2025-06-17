// main.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <pwd.h>

#include "utils.h"
#include "parser.h"
#include "builtins.h"
#include "config.h"

static void free_arguments(char **args)
{
    if (!args) return;
    for (size_t i = 0; args[i]; ++i)
        free(args[i]);
    free(args);
}

static void execute_external(char **args)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror(COLOR_RED "fork failed" COLOR_RESET);
        return;
    }
    if (pid == 0) {
        /* child */
        execvp(args[0], args);
        perror(COLOR_RED "execvp failed" COLOR_RESET);
        _exit(EXIT_FAILURE);
    }
    /* parent */
    int status;
    waitpid(pid, &status, 0);
}

int main(void)
{
    /* init config and greeting */
    config_init();
    print_help();

    while (1) {
        /* Build prompt "user@cwd$ " */
        char *user = getenv("USER");
        if (!user) {
            struct passwd *pw = getpwuid(getuid());
            user = pw ? pw->pw_name : "unknown";
        }

        char cwd[PATH_MAX];
        if (!getcwd(cwd, sizeof(cwd))) {
            strcpy(cwd, "?");
        }

        /* shorten home directory to ~ */
        const char *home = getenv("HOME");
        char display_cwd[PATH_MAX];
        if (home && strncmp(cwd, home, strlen(home)) == 0) {
            snprintf(display_cwd, sizeof(display_cwd), "~%s", cwd + strlen(home));
        } else {
            strncpy(display_cwd, cwd, sizeof(display_cwd));
            display_cwd[sizeof(display_cwd)-1] = '\0';
        }

        const char *ucol = cfg_user_color();
        const char *pcol = cfg_path_color();

        char prompt[PATH_MAX + 128];
        snprintf(prompt, sizeof(prompt), "%s%s%s@%s%s%s$ ",
                 ucol, user, COLOR_RESET,
                 pcol, display_cwd, COLOR_RESET);

        char *line = readline(prompt);
        if (!line) { /* EOF (Ctrl-D) */
            putchar('\n');
            break;
        }

        /* ignore empty line (spaces/tabs) */
        char *trim = line;
        while (*trim == ' ' || *trim == '\t') ++trim;
        if (*trim == '\0') {
            free(line);
            continue;
        }

        add_history(line);

        int argc = 0;
        char **args = parse_input(trim, &argc);
        if (!args || argc == 0) {
            free(line);
            free_arguments(args);
            continue;
        }

        int builtin_res = execute_builtin(args);
        if (builtin_res == BUILTIN_EXIT) {
            free(line);
            free_arguments(args);
            break;
        } else if (builtin_res == BUILTIN_NOT_FOUND) {
            execute_external(args);
        }

        free_arguments(args);
        free(line);
    }
    return 0;
} 