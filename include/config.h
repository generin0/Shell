// config.h
#ifndef CONFIG_H
#define CONFIG_H

/* Инициализация конфигурации. Нужно вызвать один раз при старте. */
void config_init(void);

/* Геттеры для цветов prompt. Возвращают ANSI последовательности (или "" для без цвета). */
const char *cfg_user_color(void);
const char *cfg_path_color(void);

#endif /* CONFIG_H */ 