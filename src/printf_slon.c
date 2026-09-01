#include <stdarg.h> /* va_list, va_start, va_end — работа с variadic-аргументами */
#include <stdio.h>  /* printf, putchar, fflush, vsnprintf */
#include <unistd.h> /* usleep — задержка в микросекундах */

void printf_slon(unsigned ms, const char *fmt, ...) {
    char buf[1024]; /* буфер для отформатированной строки */
    va_list args;   /* список variadic-аргументов */
    va_start(args, fmt);
    /* vsnprintf форматирует по шаблону fmt из args в buf.
       sizeof(buf) защищает от переполнения буфера. */
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    /* Походим по каждому символу до завершающего '\0'. */
    for (int i = 0; buf[i]; i++) {
        putchar(buf[i]);   /* выводим один символ */
        fflush(stdout);    /* сбрасываем буфер — без этого символы батчатся
                              из-за line-buffering stdout */
        usleep(ms * 1000); /* пауза: мс -> мкс (умножение на 1000) */
    }
}
