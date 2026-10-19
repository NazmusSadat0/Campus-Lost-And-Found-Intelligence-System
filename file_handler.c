#include "file_handler.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static int readLine(FILE *file, char *buf, size_t size)
{
    char tmp[512];
    size_t len;
    if (!fgets(tmp, sizeof tmp, file))
        return 0;
    len = strlen(tmp);
    while (len > 0 && (tmp[len - 1] == '\n' || tmp[len - 1] == '\r'))
    {
        tmp[--len] = '\0';
    }
    if (len >= size)
        return 0;
    memcpy(buf, tmp, len + 1);
    return 1;
}

int loadItems(const char *path, Item items[], int *count)
{
    FILE *file = fopen(path, "r");
    char line[256];
    int n = 0;

    if (!file)
    {
        if (errno == ENOENT)
        {
            *count = 0;
            return 1;
        }
        perror("Cannot open item text file");
        return 0;
    }

    if (!readLine(file, line, sizeof line) ||
        sscanf(line, "%d", &n) != 1 || n < 0 || n > MAX_ITEMS)
    {
        fclose(file);
        fprintf(stderr, "Invalid header in %s.\n", path);
        return 0;
    }

    for (int i = 0; i < n; i++)
    {
        Item it = {0};
        if (!readLine(file, line, sizeof line) ||
            sscanf(line, "%d %d %d %d %d",
                   &it.id, &it.type, &it.category, &it.location, &it.status) != 5 ||
            !readLine(file, it.name, sizeof it.name) ||
            !readLine(file, it.colour, sizeof it.colour) ||
            !readLine(file, it.date, sizeof it.date) ||
            !readLine(file, it.description, sizeof it.description) ||
            !readLine(file, it.reporter, sizeof it.reporter) ||
            !readLine(file, it.contact, sizeof it.contact) ||
            !validItem(&it) || findIndex(items, i, it.id) >= 0)
        {
            fclose(file);
            fprintf(stderr, "Invalid item record in %s.\n", path);
            return 0;
        }
        items[i] = it;
    }

    fclose(file);
    *count = n;
    return 1;
}

int saveItems(const char *path, const Item items[], int count)
{
    FILE *file;
    if (count < 0 || count > MAX_ITEMS)
        return 0;

    file = fopen(path, "w");
    if (!file)
    {
        perror("Cannot save item text file");
        return 0;
    }

    if (fprintf(file, "%d\n", count) < 0)
    {
        fclose(file);
        return 0;
    }

    for (int i = 0; i < count; i++)
    {
        const Item *p = &items[i];
        if (fprintf(file, "%d %d %d %d %d\n%s\n%s\n%s\n%s\n%s\n%s\n",
                    p->id, p->type, p->category, p->location, p->status,
                    p->name, p->colour, p->date, p->description,
                    p->reporter, p->contact) < 0)
        {
            fclose(file);
            return 0;
        }
    }

    return fclose(file) == 0;
}

int loadAdminData(const char *path, AdminData *admin)
{
    FILE *file = fopen(path, "r");
    char line[256];
    AdminData temp;

    initDefaultAdminData(admin);
    if (!file)
    {
        if (errno == ENOENT)
            return 1;
        perror("Cannot open admin text file");
        return 0;
    }

    memset(&temp, 0, sizeof temp);
    if (!readLine(file, line, sizeof line) ||
        sscanf(line, "%d", &temp.kwCount) != 1 ||
        temp.kwCount < 0 || temp.kwCount > MAX_KEYWORDS)
    {
        fclose(file);
        return 0;
    }

    for (int i = 0; i < temp.kwCount; i++)
    {
        if (!readLine(file, temp.keywords[i], sizeof temp.keywords[i]) ||
            !validString(temp.keywords[i], sizeof temp.keywords[i]))
        {
            fclose(file);
            return 0;
        }
    }

    if (!readLine(file, line, sizeof line) ||
        sscanf(line, "%d", &temp.fraudCount) != 1 ||
        temp.fraudCount < 0 || temp.fraudCount > MAX_FRAUDS)
    {
        fclose(file);
        return 0;
    }

    for (int i = 0; i < temp.fraudCount; i++)
    {
        FraudReport *f = &temp.frauds[i];
        if (!readLine(file, line, sizeof line) ||
            sscanf(line, "%d %d %d", &f->id, &f->itemId, &f->status) != 3 ||
            !readLine(file, f->suspectName, sizeof f->suspectName) ||
            !readLine(file, f->accuserName, sizeof f->accuserName) ||
            !readLine(file, f->accuserContact, sizeof f->accuserContact) ||
            !readLine(file, f->date, sizeof f->date) ||
            !readLine(file, f->reason, sizeof f->reason))
        {
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    *admin = temp;
    setActiveAdminData(admin);
    return 1;
}

int saveAdminData(const char *path, const AdminData *admin)
{
    FILE *file = fopen(path, "w");
    if (!file)
    {
        perror("Cannot save admin text file");
        return 0;
    }

    fprintf(file, "%d\n", admin->kwCount);
    for (int i = 0; i < admin->kwCount; i++)
    {
        fprintf(file, "%s\n", admin->keywords[i]);
    }

    fprintf(file, "%d\n", admin->fraudCount);
    for (int i = 0; i < admin->fraudCount; i++)
    {
        const FraudReport *f = &admin->frauds[i];
        fprintf(file, "%d %d %d\n%s\n%s\n%s\n%s\n%s\n",
                f->id, f->itemId, f->status,
                f->suspectName, f->accuserName, f->accuserContact,
                f->date, f->reason);
    }

    return fclose(file) == 0;
}