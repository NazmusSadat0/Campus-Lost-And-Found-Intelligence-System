#include "items.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

/*
 * Binary data is intended for the same compiler/platform build.
 * Validate the entire file before accepting its records.
 */
int loadItems(const char *path, Item items[], int *count)
{
    FILE *file = fopen(path, "rb");

    Item loaded[MAX_ITEMS];
    char magic[8];

    int n;
    int ok = 1;

    if (!file)
    {
        if (errno == ENOENT)
        {
            /* First run: no data file exists yet. */
            *count = 0;
            return 1;
        }

        perror("Cannot open data file");
        return 0;
    }

    if (fread(magic, 1, 8, file) != 8 ||
        memcmp(magic, "CLFIS01\n", 8) != 0 ||
        fread(&n, sizeof n, 1, file) != 1 ||
        n < 0 ||
        n > MAX_ITEMS)
    {

        ok = 0;
    }

    if (ok &&
        fread(loaded, sizeof(Item), (size_t)n, file) !=
            (size_t)n)
    {

        ok = 0;
    }

    if (ok && (fgetc(file) != EOF || ferror(file)))
    {
        ok = 0;
    }

    if (fclose(file) != 0)
    {
        ok = 0;
    }

    if (ok)
    {
        for (int i = 0; i < n; i++)
        {
            if (!validItem(&loaded[i]) ||
                findIndex(loaded, i, loaded[i].id) >= 0)
            {

                ok = 0;
                break;
            }
        }
    }

    if (!ok)
    {
        fprintf(
            stderr,
            "Invalid or unreadable data file. "
            "Existing file left unchanged.\n");

        return 0;
    }

    memcpy(items, loaded, (size_t)n * sizeof(Item));
    *count = n;

    return 1;
}

int saveItems(const char *path, const Item items[], int count)
{
    char temporary[512];

    int ok;
    FILE *file;

    if (count < 0 || count > MAX_ITEMS)
    {
        return 0;
    }

    for (int i = 0; i < count; i++)
    {
        if (!validItem(&items[i]) ||
            findIndex(items, i, items[i].id) >= 0)
        {

            return 0;
        }
    }

    if (snprintf(
            temporary,
            sizeof temporary,
            "%s.tmp",
            path) >= (int)sizeof temporary)
    {

        return 0;
    }

    /*
     * Write a temporary file first, so an ordinary write
     * failure does not destroy the previous data file.
     */
    file = fopen(temporary, "wb");

    if (!file)
    {
        perror("Cannot save data");
        return 0;
    }

    ok =
        fwrite("CLFIS01\n", 1, 8, file) == 8 &&

        fwrite(&count, sizeof count, 1, file) == 1 &&

        fwrite(
            items,
            sizeof(Item),
            (size_t)count,
            file) == (size_t)count;

    if (fclose(file) != 0)
    {
        ok = 0;
    }

    if (!ok)
    {
        fprintf(stderr, "Save failed while writing.\n");
        remove(temporary);
        return 0;
    }

#ifdef _WIN32
    /* Windows: move the temporary file over the old data file. */
    if (!MoveFileExA(
            temporary,
            path,
            MOVEFILE_REPLACE_EXISTING))
    {

        fprintf(
            stderr,
            "Cannot replace data file. Windows error: %lu\n",
            (unsigned long)GetLastError());

        fprintf(
            stderr,
            "New data remains in %s. Save was not completed.\n",
            temporary);

        return 0;
    }
#else
    /* Linux / macOS */
    if (rename(temporary, path) != 0)
    {
        perror("Cannot replace data file");

        fprintf(
            stderr,
            "New data remains in %s. Save was not completed.\n",
            temporary);

        return 0;
    }
#endif
    return 1;
}