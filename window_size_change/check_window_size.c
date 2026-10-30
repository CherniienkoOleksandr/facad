#define _POSIX_C_SOURCE 200809L /* без этого -std=c11 (strict ANSI) прячет sigaction */

/**
 * @file check_window_size.c
 * @brief Перехват изменения размера окна терминала.
 *
 * Хендлер SIGWINCH только выставляет флаг (async-signal-safe), а вся работа —
 * ioctl, сравнение размеров и печать — выполняется в обычном контексте,
 * в цикле по pause().
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include "check_window_size.h"

static volatile sig_atomic_t changed;

static void on_winch(int sig)
{
    (void)sig;
    changed = 1;
}

int win_get_size(struct win_size *ws)
{
    struct winsize w;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0)
        return -1;

    ws->cols = w.ws_col;
    ws->rows = w.ws_row;
    return 0;
}

static void print_size(const struct win_size *ws)
{
    char ts[16];
    time_t now = time(NULL);

    strftime(ts, sizeof(ts), "%H:%M:%S", localtime(&now));
    printf("[%s] cols=%d rows=%d\n", ts, ws->cols, ws->rows);
    fflush(stdout);
}

int window_watch(win_callback cb, int min_delta)
{
    struct win_size ws;
    struct win_size prev;
    struct sigaction sa = {0};
    struct sigaction old = {0};

    sa.sa_handler = on_winch;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; /* без SA_RESTART: pause() должен возвращать управление */

    if (sigaction(SIGWINCH, &sa, &old) == -1)
        return -1;

    if (win_get_size(&prev) == -1) {
        sigaction(SIGWINCH, &old, NULL);
        return -1;
    }

    for (;;) {
        changed = 0;
        while (!changed)
            pause(); /* ждём SIGWINCH */

        if (win_get_size(&ws) == -1) {
            sigaction(SIGWINCH, &old, NULL);
            return 0; /* терминал закрылся — штатный выход */
        }

        /* Сравниваем с последним отрисованным размером: мелкие изменения
         * накапливаются, а ложные сигналы (размер тот же) ничего не стоят. */
        if (abs(ws.cols - prev.cols) > min_delta ||
            abs(ws.rows - prev.rows) > min_delta) {
            prev = ws;
            print_size(&ws);
            cb(&ws);
        }
    }
}
