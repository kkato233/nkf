#define _XOPEN_SOURCE 700

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "nkf32so.h"

static int write_file(const char *path, const char *content)
{
    FILE *fp = fopen(path, "wb");
    if (fp == NULL) return -1;
    if (fwrite(content, 1, strlen(content), fp) != strlen(content)) {
        fclose(fp);
        return -1;
    }
    return fclose(fp);
}

static int file_matches(const char *path, const char *content)
{
    char buf[64];
    size_t expected = strlen(content);
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) return 0;
    size_t actual = fread(buf, 1, sizeof(buf), fp);
    int ok = actual == expected && memcmp(buf, content, expected) == 0 &&
        !ferror(fp);
    if (fclose(fp) != 0) ok = 0;
    return ok;
}

static int directory_has_only_test_files(const char *path)
{
    DIR *dir = opendir(path);
    struct dirent *entry;
    int found_input = 0;
    int found_tmp = 0;
    int valid = 1;
    if (dir == NULL) return 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (strcmp(entry->d_name, "input.txt") == 0) {
            found_input = 1;
        } else if (strcmp(entry->d_name, "tmp") == 0) {
            found_tmp = 1;
        } else {
            valid = 0;
        }
    }
    if (closedir(dir) != 0) return 0;
    return valid && found_input && found_tmp;
}

static int directory_is_empty(const char *path)
{
    DIR *dir = opendir(path);
    struct dirent *entry;
    int empty = 1;
    if (dir == NULL) return 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            empty = 0;
            break;
        }
    }
    if (closedir(dir) != 0) return 0;
    return empty;
}

int main(void)
{
    char root_template[] = "/tmp/nkf32so-test-XXXXXX";
    char *root = mkdtemp(root_template);
    char tempdir[256];
    char filepath[256];
    char missing_tmpdir[256];
    struct stat st;
    const char content[] = "ASCII text\n";
    int result = 1;

    if (root == NULL) {
        perror("mkdtemp");
        return 1;
    }
    snprintf(tempdir, sizeof(tempdir), "%s/tmp", root);
    snprintf(filepath, sizeof(filepath), "%s/input.txt", root);
    snprintf(missing_tmpdir, sizeof(missing_tmpdir), "%s/missing", root);
    if (mkdir(tempdir, 0700) != 0 || write_file(filepath, content) != 0 ||
        chmod(filepath, 0644) != 0 || setenv("TMPDIR", tempdir, 1) != 0) {
        perror("prepare test files");
        goto cleanup;
    }

    if (!NkfFileConvert1Safe(filepath, (DWORD)strlen(filepath) + 1)) {
        fprintf(stderr, "conversion failed for a readable input\n");
        goto cleanup;
    }
    if (!file_matches(filepath, content) || stat(filepath, &st) != 0 ||
        (st.st_mode & 0777) != 0600 || !directory_is_empty(tempdir) ||
        !directory_has_only_test_files(root)) {
        fprintf(stderr, "conversion output, mode, or temporary-file cleanup is incorrect\n");
        goto cleanup;
    }

    if (chmod(filepath, 0644) != 0 || setenv("TMPDIR", missing_tmpdir, 1) != 0) {
        perror("prepare failure case");
        goto cleanup;
    }
    if (NkfFileConvert1Safe(filepath, (DWORD)strlen(filepath) + 1)) {
        fprintf(stderr, "conversion unexpectedly succeeded with an invalid TMPDIR\n");
        goto cleanup;
    }
    if (!file_matches(filepath, content) || stat(filepath, &st) != 0 ||
        (st.st_mode & 0777) != 0644) {
        fprintf(stderr, "failed conversion changed the input file\n");
        goto cleanup;
    }

    result = 0;

cleanup:
    unsetenv("TMPDIR");
    unlink(filepath);
    rmdir(tempdir);
    rmdir(root);
    return result;
}
