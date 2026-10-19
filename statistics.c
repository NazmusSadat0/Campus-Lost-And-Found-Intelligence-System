#include "statistics.h"

#include <stdio.h>
#include <stdlib.h>

void showStatistics(const Item items[], int count, const AdminData *admin)
{
    int totalLost = 0, totalFound = 0, totalOpen = 0;
    int totalResolved = 0, totalBlacklisted = 0;
    int topLostCat = 0, topFoundCat = 0, topLoc = 0;

    /*
     * Dynamic memory allocation (calloc) to store category and location
     * statistical counters initialized to zero on the heap.
     */
    GroupStat *catStats = (GroupStat *)calloc(CATEGORY_COUNT, sizeof(GroupStat));
    GroupStat *locStats = (GroupStat *)calloc(LOCATION_COUNT, sizeof(GroupStat));

    if (!catStats || !locStats)
    {
        fprintf(stderr, "Memory allocation failed for statistics.\n");
        free(catStats);
        free(locStats);
        return;
    }

    /* Traverse items array using pointer arithmetic */
    for (const Item *p = items; p < items + count; p++)
    {
        if (p->status == BLACKLISTED)
        {
            totalBlacklisted++;
            continue;
        }

        int c = p->category - 1;
        int l = p->location - 1;

        catStats[c].total++;
        locStats[l].total++;

        if (p->type == LOST)
        {
            totalLost++;
            catStats[c].lost++;
            locStats[l].lost++;
        }
        else
        {
            totalFound++;
            catStats[c].found++;
            locStats[l].found++;
        }

        if (p->status == RESOLVED)
        {
            totalResolved++;
            catStats[c].resolved++;
            locStats[l].resolved++;
        }
        else
        {
            totalOpen++;
        }
    }

    int publicCount = totalLost + totalFound;
    double overallRecovery = publicCount > 0
                                 ? (100.0 * totalResolved) / publicCount
                                 : 0.0;

    puts("\n========================================================");
    puts("      CAMPUS LOST & FOUND STATISTICS & TREND REPORT     ");
    puts("========================================================");
    printf("Total Public Reports : %d (Lost: %d | Found: %d)\n",
           publicCount, totalLost, totalFound);
    printf("Resolution Status    : Open: %d | Recovered/Resolved: %d (%.1f%%)\n",
           totalOpen, totalResolved, overallRecovery);
    printf("Blacklisted Flagged  : %d report(s) intercepted\n", totalBlacklisted);
    printf("Fraud Allegations    : %d total submitted\n", admin->fraudCount);

    puts("\n--- CATEGORY-WISE LOST/FOUND & RECOVERY TRENDS ---");
    printf("%-20s %6s %6s %8s %6s %10s\n",
           "Category", "Lost", "Found", "Resolved", "Total", "Recovery%");
    for (int i = 0; i < CATEGORY_COUNT; i++)
    {
        double rate = catStats[i].total > 0
                          ? (100.0 * catStats[i].resolved) / catStats[i].total
                          : 0.0;
        printf("%-20s %6d %6d %8d %6d %9.1f%%\n",
               categories[i], catStats[i].lost, catStats[i].found,
               catStats[i].resolved, catStats[i].total, rate);
        if (catStats[i].lost > catStats[topLostCat].lost)
            topLostCat = i;
        if (catStats[i].found > catStats[topFoundCat].found)
            topFoundCat = i;
    }

    puts("\n--- LOCATION-WISE HOTSPOTS & RECOVERY PATTERNS ---");
    printf("%-20s %6s %6s %8s %6s %10s\n",
           "Location", "Lost", "Found", "Resolved", "Total", "Recovery%");
    for (int i = 0; i < LOCATION_COUNT; i++)
    {
        double rate = locStats[i].total > 0
                          ? (100.0 * locStats[i].resolved) / locStats[i].total
                          : 0.0;
        printf("%-20s %6d %6d %8d %6d %9.1f%%\n",
               locations[i], locStats[i].lost, locStats[i].found,
               locStats[i].resolved, locStats[i].total, rate);
        if (locStats[i].total > locStats[topLoc].total)
            topLoc = i;
    }

    puts("\n--- KEY INTELLIGENCE INSIGHTS ---");
    if (publicCount > 0)
    {
        printf("* Most Frequently Lost Category  : %s (%d lost)\n",
               categories[topLostCat], catStats[topLostCat].lost);
        printf("* Most Frequently Found Category : %s (%d found)\n",
               categories[topFoundCat], catStats[topFoundCat].found);
        printf("* Top Campus Hotspot Location    : %s (%d total reports)\n",
               locations[topLoc], locStats[topLoc].total);
    }
    else
    {
        puts("* Not enough public reports yet to compute trend leaders.");
    }

    free(catStats);
    free(locStats);
}