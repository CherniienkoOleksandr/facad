#include <stdarg.h> /* va_list, va_start, va_end — работа с variadic-аргументами */
#include <stdio.h>
#include <unistd.h>

#include "config.h"
#include "src/dir_analytics.h"
#include "src/printf_slon.h"

#define delay 10

/*********** printf simple ( char-to-char style )   **********/
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
/***************************************************************************/

/*********  Dir analytics printing. Slon ver Temporary version **************/
void slon_dir_analytics(const char *path, unsigned delay_ms) {

    if (!delay_ms)
        delay_ms = delay;

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        print_dir_analytics(path);
        return;
    }

    int saved_stdout = dup(STDOUT_FILENO);

    if (saved_stdout == -1) {
        close(pipefd[0]);
        close(pipefd[1]);

        print_dir_analytics(path);
        return;
    }

    fflush(stdout);

    if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        close(saved_stdout);

        print_dir_analytics(path);
        return;
    }

    close(pipefd[1]);

    print_dir_analytics(path);

    fflush(stdout);

    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);

    char c;
    ssize_t n;

    while ((n = read(pipefd[0], &c, 1)) > 0) {
        usleep(delay_ms * 1000);

        write(STDOUT_FILENO, &c, 1);
    }

    close(pipefd[0]);
}
/*******************************************************************/
void print_version2(unsigned delay_ms) {

    if (!delay_ms)
        delay_ms = delay;
    printf_slon(delay_ms, "facad version %s\n",
                VERSION); // Print version number
}
