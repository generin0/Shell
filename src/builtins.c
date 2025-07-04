// builtins.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>

#include "builtins.h"
#include "utils.h"

/* Prototypes for individual builtin commands */
static int sh_exit(char **args);
static int sh_cd(char **args);
static int sh_pwd(char **args);
static int sh_clear(char **args);
static int sh_echo(char **args);
static int sh_mytime(char **args);
static int sh_mkdir(char **args);
static int sh_rmdir(char **args);
static int sh_help(char **args);
static int sh_cat(char **args);
static int sh_whoami(char **args);
static int sh_touch(char **args);
static int sh_rm(char **args);
static int sh_ls(char **args);
static int sh_mv(char **args);
static int sh_sleep(char **args);
static int sh_env(char **args);

/* Table describing all builtin commands */
static struct {
    const char *name;
    int (*func)(char **args);
} builtin_table[] = {
    {"exit",  sh_exit},
    {"cd",    sh_cd},
    {"pwd",   sh_pwd},
    {"clear", sh_clear},
    {"echo",  sh_echo},
    {"mytime",sh_mytime},
    {"mkdir", sh_mkdir},
    {"rmdir", sh_rmdir},
    {"help",  sh_help},
    {"cat",   sh_cat},
    {"whoami",sh_whoami},
    {"touch", sh_touch},
    {"rm",    sh_rm},
    {"ls",    sh_ls},
    {"mv",    sh_mv},
    {"sleep", sh_sleep},
    {"env",   sh_env}
};

#define BUILTIN_COUNT (sizeof(builtin_table) / sizeof(builtin_table[0]))

int execute_builtin(char **args)
{
    if (!args || !args[0])
        return BUILTIN_NOT_FOUND;

    for (size_t i = 0; i < BUILTIN_COUNT; ++i) {
        if (strcmp(args[0], builtin_table[i].name) == 0) {
            return builtin_table[i].func(args);
        }
    }
    return BUILTIN_NOT_FOUND;
}

/* ==== Implementation of individual built-ins ==== */
static int sh_exit(char **args)
{
    (void)args; /* unused */
    printf(COLOR_RED "Closing the shell.\n" COLOR_RESET);
    return BUILTIN_EXIT;
}

static int sh_env(char **args)
{
    (void)args;
    extern char **environ;
    for (char **env = environ; *env; env++) {
        printf("%s\n", *env);
    }
    return BUILTIN_SUCCESS;
}

static int sh_sleep(char **args)
{
    if (!args[1]) {
        printf(COLOR_GREEN"Usage: sleep <seconds>\n"COLOR_RESET);
        return BUILTIN_SUCCESS;
    }
    
    int sec = atoi(args[1]);

    if (sec >= 1000) {
        printf(COLOR_RED"Too much seconds\n"COLOR_RESET);
        return BUILTIN_EXIT;
    }

    sleep(sec);
    return BUILTIN_SUCCESS;
}

static int sh_mv(char **args)
{
    if (!args[1] || !args[2]) {
        printf(COLOR_GREEN"Usage: mv <source> <destination>\n"COLOR_RESET);
        return BUILTIN_SUCCESS;
    }
    
    if (rename(args[1], args[2]) == 0) {
        printf(COLOR_GREEN"File '%s' is successfully moved to -> %s\n"COLOR_RESET, args[1], args[2]);
        return BUILTIN_SUCCESS;
    } else {
        printf(COLOR_RED"Error moving a file."COLOR_RESET);
        return BUILTIN_EXIT;
    }

    return BUILTIN_SUCCESS;
}

static int sh_ls(char **args)
{
    (void)args;
    DIR *dir = opendir(".");
    if (!dir) {
        perror("ls");
        return BUILTIN_EXIT;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_name[0] == '.') continue;

        struct stat st;
        stat(entry->d_name, &st);

        if (S_ISDIR(st.st_mode)) {
            printf(COLOR_BLUE "%s  " COLOR_RESET, entry->d_name);
        } else if (st.st_mode & S_IXUSR) {
            printf(COLOR_GREEN "%s  " COLOR_RESET, entry->d_name);
        } else {
            printf("%s  ", entry->d_name);
        }
    }
    printf("\n");
    closedir(dir);
    return BUILTIN_SUCCESS;
}

static int sh_rm(char **args) 
{
    if (args[1] == NULL) {
        printf(COLOR_GREEN"Usage: rm <f>"COLOR_RESET);
        return BUILTIN_SUCCESS;
    } 
    
    if (unlink(args[1]) == 0) {
        printf("File %s removed successfully.\n", args[1]);
        return BUILTIN_SUCCESS;
    } else {
        perror(COLOR_RED"Error removing file:"COLOR_RESET);
        return BUILTIN_EXIT;
    }
}

static int sh_touch(char **args) 
{
    FILE *file = fopen(args[1], "a");
    if (file == NULL) {
        printf(COLOR_RED"Error creating a file.\n"COLOR_RESET);
        return BUILTIN_EXIT;
    } else {
        printf("File -%s is created successfully.\n", args[1]);
        return BUILTIN_SUCCESS;
    }
}

static int sh_whoami(char **args)
{
    (void)args;
    char *user = getenv("USER");
    if (user) {
        printf(COLOR_GREEN"%s\n"COLOR_RESET, user);
    } else {
        printf(COLOR_RED"Cannot find a user.\n"COLOR_RESET);
        return BUILTIN_EXIT;
    }
    return BUILTIN_SUCCESS;
}

static int sh_cd(char **args)
{
    if (!args[1]) {
        const char *home = getenv("HOME");
        if (!home)
            home = "/";
        chdir(home);
    } else {
        if (chdir(args[1]) != 0) {
            perror(COLOR_RED "cd failed" COLOR_RESET);
        }
    }
    return BUILTIN_SUCCESS;
}

static int sh_pwd(char **args)
{
    (void)args;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        printf("%s\n", cwd);
    } else {
        perror(COLOR_RED "pwd failed" COLOR_RESET);
    }
    return BUILTIN_SUCCESS;
}

static int sh_cat(char **args)
{
    FILE *file = fopen(args[1], "r");
    if (file == NULL) {
        printf("Cannot open the %s file.\n", args[1]);
        return BUILTIN_EXIT;
    }
    int ch;
    while ((ch = getc(file)) != EOF) {
      putchar(ch);
    }
    fclose(file);
    return BUILTIN_SUCCESS;
}

static int sh_clear(char **args)
{
    (void)args;
    /* ANSI escape sequence to clear screen */
    printf("\033[2J\033[H");
    return BUILTIN_SUCCESS;
}

static int sh_echo(char **args)
{
    for (int i = 1; args[i]; ++i) {
        printf("%s", args[i]);
        if (args[i + 1])
            putchar(' ');
    }
    putchar('\n');
    return BUILTIN_SUCCESS;
}

static int sh_mytime(char **args)
{
    (void)args;
    time_t current_time = time(NULL);
    if (current_time == (time_t)-1) {
        perror(COLOR_RED "time failed" COLOR_RESET);
        return BUILTIN_SUCCESS;
    }

    struct tm *tm_info = localtime(&current_time);
    if (!tm_info) {
        perror(COLOR_RED "localtime failed" COLOR_RESET);
        return BUILTIN_SUCCESS;
    }

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("%s\n", buffer);
    return BUILTIN_SUCCESS;
}

static int sh_mkdir(char **args)
{
    if (!args[1]) {
        fprintf(stderr, "Usage: mkdir <dir>\n");
        return BUILTIN_SUCCESS;
    }

    if (mkdir(args[1], 0755) == 0) {
        printf(COLOR_GREEN "Directory %s successfully created.\n" COLOR_RESET, args[1]);
    } else {
        perror(COLOR_RED "mkdir failed" COLOR_RESET);
    }
    return BUILTIN_SUCCESS;
}

static int sh_rmdir(char **args)
{
    if (!args[1]) {
        fprintf(stderr, "Usage: rmdir <dir>\n");
        return BUILTIN_SUCCESS;
    }

    if (rmdir(args[1]) == 0) {
        printf(COLOR_GREEN "Successfully deleted %s directory.\n" COLOR_RESET, args[1]);
    } else {
        perror(COLOR_RED "Cannot delete directory" COLOR_RESET);
    }
    return BUILTIN_SUCCESS;
}

void print_help(void)
{
    puts("");
    printf(COLOR_GREEN "═══ SHELL COMMANDS ═══\n" COLOR_RESET);
    printf(COLOR_CYAN  "Built-in commands:\n" COLOR_RESET);
    printf("  help        - Show this help\n");
    printf("  exit        - Exit the shell\n");
    printf("  cd <d>      - Change directory\n");
    printf("  pwd         - Print working directory\n");
    printf("  clear       - Clear screen\n");
    printf("  mytime      - Show current date and time\n");
    printf("  echo <t>    - Print text\n");
    printf("  mkdir <d>   - Create directory\n");
    printf("  rmdir <d>   - Remove empty directory\n");
    printf("  cat <f>     - Show file contents\n");
    printf("  whoami      - Shows the current user\n");
    printf("  touch <f>   - Creates a file\n");
    printf("  rm <f>      - Deletes a file\n");
    printf("  ls          - Show the files of current directory\n");
    printf("  mv <f><des> - Moves the file\n");
    printf("  sleep <sec> - Executing delay\n");
    printf("  env         - Shows the current environments\n");
    printf(COLOR_GREEN "External commands are also supported!\n" COLOR_RESET);
    puts("");
}

static int sh_help(char **args)
{
    (void)args;
    print_help();
    return BUILTIN_SUCCESS;
}
