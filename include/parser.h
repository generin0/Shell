// parser.h
#ifndef PARSER_H
#define PARSER_H

/* Разбирает входную строку `input` на аргументы (разделители — пробелы/табуляции).
 * Возвращает массив строк, заканчивающийся NULL. Количество аргументов
 * записывается в *argc_out, если указатель не NULL.
 * Массив и строки внутри нужно освободить через free() после использования.
 */
char **parse_input(const char *input, int *argc_out);

#endif /* PARSER_H */ 