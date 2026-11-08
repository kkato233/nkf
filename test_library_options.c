#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "nkf32so.h"

static int
check_option(const char *option)
{
    pid_t pid = fork();
    int status;

    if (pid < 0) return 0;
    if (pid == 0) {
        char option_copy[16];
        char out[32];
        int bytes_returned;
        int result;

        if (freopen("/dev/null", "w", stdout) == NULL) _exit(44);
        strcpy(option_copy, option);
        SetNkfOption(option_copy);
        result = NkfConvertSafe(out, sizeof(out), &bytes_returned, "test", 4);
        _exit(result == FALSE ? 42 : 43);
    }

    if (waitpid(pid, &status, 0) < 0) return 0;
    return WIFEXITED(status) && WEXITSTATUS(status) == 42;
}

int
main(void)
{
    static const char *options[] = {
        "--help",
        "--version",
        "-v",
        "-V"
    };
    char out[32];
    int bytes_returned;
    int i;

    for (i = 0; i < sizeof(options) / sizeof(options[0]); i++) {
        if (!check_option(options[i])) {
            fprintf(stderr, "library option did not return an error: %s\n", options[i]);
            return 1;
        }
    }

    if (!NkfConvertSafe(out, sizeof(out), &bytes_returned, "test", 4)) {
        fprintf(stderr, "conversion without options failed\n");
        return 1;
    }
    return 0;
}
