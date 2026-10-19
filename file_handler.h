#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "items.h"
#include "admin.h"

int loadItems(const char *path, Item items[], int *count);
int saveItems(const char *path, const Item items[], int count);

int loadAdminData(const char *path, AdminData *admin);
int saveAdminData(const char *path, const AdminData *admin);

#endif