// builtins.h
#ifndef BUILTINS_H
#define BUILTINS_H

/* Return codes */
#define BUILTIN_NOT_FOUND (-1)
#define BUILTIN_SUCCESS (0)
#define BUILTIN_EXIT (1)

/* Execute builtin if exists.
 * Returns:
 *   BUILTIN_EXIT     – попросить оболочку завершиться
 *   BUILTIN_SUCCESS  – команда выполнена
 *   BUILTIN_NOT_FOUND – не является встроенной
 */
int execute_builtin(char **args);

/* Отдельная функция для вывода справки, чтобы ею мог воспользоваться main. */
void print_help(void);

#endif /* BUILTINS_H */ 