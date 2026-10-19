#ifndef STATISTICS_H
#define STATISTICS_H

#include "items.h"
#include "admin.h"

typedef struct {
    int lost;
    int found;
    int resolved;
    int total;
} GroupStat;

void showStatistics(const Item items[], int count, const AdminData *admin);

#endif