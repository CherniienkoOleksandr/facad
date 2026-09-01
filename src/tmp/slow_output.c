#include "slow_output.h"

#include <stdio.h>
#include <unistd.h>

void slow_output(void (*func)(void), unsigned delay_ms) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        func();
        return;
    }

    int saved_stdout = dup(STDOUT_FILENO);

    if (saved_stdout == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        func();
        return;
    }

    fflush(stdout);

    /*
     * Redirect stdout to the pipe.
     */
    if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        close(saved_stdout);
        func();
        return;
    }

    close(pipefd[1]);

    /*
     * Execute the selected function.
     * Its stdout now goes into the pipe.
     */
    func();

    /*
     * Make sure all printf() data reaches the pipe.
     */
    fflush(stdout);

    /*
     * Restore the original stdout.
     */
    if (dup2(saved_stdout, STDOUT_FILENO) == -1) {
        close(saved_stdout);
        close(pipefd[0]);
        return;
    }

    close(saved_stdout);

    /*
     * Read and output one character at a time.
     */
    char c;
    ssize_t n;

    while ((n = read(pipefd[0], &c, 1)) > 0) {
        if (delay_ms)
            usleep(delay_ms * 1000);

        write(STDOUT_FILENO, &c, 1);
    }

    close(pipefd[0]);
}
