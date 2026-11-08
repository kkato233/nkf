#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FILECONVERT2
#include "nkf32so.h"

int main(void)
{
    char directory[] = "/tmp/nkf32so_fileconvert_XXXXXX";
    char input[sizeof directory + 8];
    char missing_output[sizeof directory + 20];
    char output[sizeof directory + 8];
    FILE *file = NULL;
    int result = 1;

    if (mkdtemp(directory) == NULL) return result;
    snprintf(input, sizeof input, "%s/input", directory);
    snprintf(missing_output, sizeof missing_output, "%s/missing/output", directory);
    snprintf(output, sizeof output, "%s/output", directory);

    file = fopen(input, "wb");
    if (file == NULL) goto cleanup;
    fputs("test\n", file);
    fclose(file);
    file = NULL;

    if (NkfFileConvert2Safe(input, sizeof input, missing_output, sizeof missing_output)) {
        goto cleanup;
    }
    if (!NkfFileConvert2Safe(input, sizeof input, output, sizeof output)) {
        goto cleanup;
    }

    file = fopen(output, "rb");
    if (file == NULL) goto cleanup;
    {
        char contents[8];
        if (fgets(contents, sizeof contents, file) == NULL ||
            strcmp(contents, "test\n") != 0) {
            goto cleanup;
        }
    }
    result = 0;

cleanup:
    if (file != NULL) fclose(file);
    unlink(input);
    unlink(output);
    rmdir(directory);
    return result;
}
