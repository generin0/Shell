#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "utils.h"

static const char *color_user = COLOR_GREEN;
static const char *color_path = COLOR_CYAN;

static const char *color_from_name(const char *name)
{
    if (!name) return "";
    if (strcasecmp(name, "RED") == 0) return COLOR_RED;
    if (strcasecmp(name, "GREEN") == 0) return COLOR_GREEN;
    if (strcasecmp(name, "YELLOW") == 0) return COLOR_YELLOW;
    if (strcasecmp(name, "BLUE") == 0) return COLOR_BLUE;
    if (strcasecmp(name, "MAGENTA") == 0) return COLOR_MAGENTA;
    if (strcasecmp(name, "CYAN") == 0) return COLOR_CYAN;
    if (strcasecmp(name, "RESET") == 0) return COLOR_RESET;
    if (strcasecmp(name, "NONE") == 0) return ""; /* disable color */
    return NULL; /* unknown */
}

void config_init(void)
{
    const char *home = getenv("HOME");
    if (!home) return;

    char path[1024];
    snprintf(path, sizeof(path), "%s/.myshellrc", home);
    FILE *f = fopen(path, "r");
    if (!f) return; /* no config — silence */

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        /* remove newline */
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';

        /* skip comments and blank lines */
        char *p = line;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '#' || *p == '\0') continue;

        char *eq = strchr(p, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = p;
        char *value = eq + 1;

        /* trim key trailing spaces */
        char *end = key + strlen(key) - 1;
        while (end > key && (*end == ' ' || *end == '\t')) *end-- = '\0';

        if (strcasecmp(key, "color_user") == 0) {
            const char *col = color_from_name(value);
            if (col) color_user = col;
        } else if (strcasecmp(key, "color_path") == 0) {
            const char *col = color_from_name(value);
            if (col) color_path = col;
        }
    }
    fclose(f);
}

const char *cfg_user_color(void) { return color_user; }
const char *cfg_path_color(void) { return color_path; } 