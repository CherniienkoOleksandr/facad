/**
 * @file main.c
 * @brief window_size_change — ls при старте, слежение за размером окна с --havy.
 *
 * Без флагов: просто печатает содержимое текущей директории (ls).
 * С --havy (--heavy): подключает слежение за размером окна и при каждом
 * изменении больше чем на MIN_DELTA колонок/строк повторяет ls -1.
 *
 * Сборка: make   (или cc -Wall -Wextra -std=c11 -o window_size_change *.c)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "check_window_size.h"

#define MIN_DELTA 5

static void run_ls(const char *flags)
{
    char cmd[32];

    printf("\033[H\033[2J"); /* чистим экран, иначе вывод наслаивается */
    snprintf(cmd, sizeof(cmd), "ls %s --color=auto", flags);
    fflush(stdout);

    if (system(cmd) == -1)
        perror("system");
    fflush(stdout);
}

/* Вызывается из check_window_size.c при заметном изменении окна */
static void on_resize(const struct win_size *ws)
{
    (void)ws;
    run_ls("-1");
}

int main(int argc, char **argv)
{
    struct win_size ws;
    int heavy = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--havy") == 0 || strcmp(argv[i], "--heavy") == 0)
            heavy = 1;
    }

    run_ls(""); /* ls при старте — всегда */

    if (!heavy)
        return 0;

    if (win_get_size(&ws) == -1) {
        fputs("window_size_change: --havy требует терминал\n", stderr);
        return 1;
    }

    return window_watch(on_resize, MIN_DELTA) == 0 ? 0 : 1;
}
