#ifndef CHECK_WINDOW_SIZE_H
#define CHECK_WINDOW_SIZE_H

/**
 * @file check_window_size.h
 * @brief Отслеживание изменения размера терминала (SIGWINCH).
 *
 * Используется в режиме --havy: при изменении размера окна больше чем
 * на min_delta колонок/строк вызывается переданная функция.
 */

struct win_size {
    int cols;
    int rows;
};

typedef void (*win_callback)(const struct win_size *ws);

/**
 * @brief Прочитать текущий размер терминала через ioctl(TIOCGWINSZ).
 *
 * @param ws куда записать размер
 * @return 0 при успехе, -1 если stdout не терминал
 */
int win_get_size(struct win_size *ws);

/**
 * @brief Блокирующий цикл: ждать SIGWINCH и вызывать cb при заметном ресайзе.
 *
 * Порог min_delta сравнивается с последним отрисованным размером, поэтому
 * мелкие шаги накапливаются (3 + 3 > 5 сработает). Ложные сигналы, при которых
 * размер не изменился, игнорируются.
 *
 * @param cb функция, вызываемая с новым размером окна
 * @param min_delta минимальное изменение cols/rows, при котором вызывать cb
 * @return 0 при нормальном завершении (терминал закрылся), -1 при ошибке
 */
int window_watch(win_callback cb, int min_delta);

#endif // CHECK_WINDOW_SIZE_H
