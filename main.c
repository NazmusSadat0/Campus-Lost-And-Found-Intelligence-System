#include "items.h"
#include "admin.h"
#include "matching.h"
#include "statistics.h"
#include "file_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void syncChanges(const Item items[], int count, const AdminData *admin, int *dirty)
{
    if (!*dirty)
        return;
    if (saveItems(DATA_FILE, items, count) && saveAdminData(ADMIN_FILE, admin))
    {
        *dirty = 0;
        puts("Changes saved.");
    }
    else
    {
        fprintf(stderr, "Warning: Changes could not be saved to disk.\n");
    }
}

static void runStudentPanel(Item items[], int *count, AdminData *admin, int *dirty)
{
    int choice;
    do
    {
        puts("\n--- STUDENT PANEL ---");
        puts("1. Report Lost Item");
        puts("2. Report Found Item");
        puts("3. Display Public Reports");
        puts("4. Search Reports");
        puts("5. Find Possible Matches");
        puts("6. Claim / Record Item Return");
        puts("7. Submit Fraud Allegation / Suspicious Claim");
        puts("0. Back to Main Menu");

        if (!readNumber("Choice: ", 0, 7, &choice))
            choice = 0;
        int changed = 0;

        switch (choice)
        {
        case 1:
            changed = addItem(items, count, LOST);
            break;
        case 2:
            changed = addItem(items, count, FOUND);
            break;
        case 3:
            displayItems(items, *count, 0);
            break;
        case 4:
            searchItems(items, *count, 0);
            break;
        case 5:
            findMatches(items, *count);
            break;
        case 6:
            changed = recordReturn(items, *count);
            break;
        case 7:
            changed = submitFraudAllegation(items, *count, admin);
            break;
        case 0:
            break;
        }

        if (changed)
            *dirty = 1;
        syncChanges(items, *count, admin, dirty);
    } while (choice != 0);
}

static void runAdminPanel(Item items[], int *count, AdminData *admin, int *dirty)
{
    char pin[32];
    if (!readText("Enter Admin PIN (default 1234): ", pin, sizeof pin))
        return;
    if (strcmp(pin, ADMIN_PIN) != 0)
    {
        puts("Incorrect Admin PIN. Access denied.");
        return;
    }

    int choice;
    do
    {
        puts("\n--- ADMIN PANEL ---");
        puts("1. Display All Reports (Including Blacklisted)");
        puts("2. Search All Reports");
        puts("3. Update Report");
        puts("4. Delete Report");
        puts("5. Review Blacklisted Item Reports");
        puts("6. Manage Blacklist Keywords");
        puts("7. Review Fraud Allegations");
        puts("8. View Statistics & Lost/Found Trends");
        puts("9. Record Item Return");
        puts("0. Back to Main Menu");

        if (!readNumber("Choice: ", 0, 9, &choice))
            choice = 0;
        int changed = 0;

        switch (choice)
        {
        case 1:
            displayItems(items, *count, 1);
            break;
        case 2:
            searchItems(items, *count, 1);
            break;
        case 3:
            changed = updateItem(items, *count, admin);
            break;
        case 4:
            changed = deleteItem(items, count);
            break;
        case 5:
            changed = reviewBlacklistedItems(items, count);
            break;
        case 6:
            changed = manageBlacklist(admin, items, *count);
            break;
        case 7:
            changed = reviewFraudAllegations(items, *count, admin);
            break;
        case 8:
            showStatistics(items, *count, admin);
            break;
        case 9:
            changed = recordReturn(items, *count);
            break;
        case 0:
            break;
        }

        if (changed)
            *dirty = 1;
        syncChanges(items, *count, admin, dirty);
    } while (choice != 0);
}

int main(void)
{
    static Item items[MAX_ITEMS];
    static AdminData admin;
    int count = 0, choice, dirty = 0;

    srand((unsigned int)time(NULL));

    if (!loadItems(DATA_FILE, items, &count))
    {
        return EXIT_FAILURE;
    }
    loadAdminData(ADMIN_FILE, &admin);

    printf("Loaded %d report(s) and %d blacklist keyword(s).\n", count, admin.kwCount);

    do
    {
        puts("\n==========================================");
        puts("  CAMPUS LOST & FOUND INTELLIGENCE SYSTEM ");
        puts("==========================================");
        puts("1. Student Panel");
        puts("2. Admin Panel");
        puts("0. Save and Exit");

        if (!readNumber("Select Panel: ", 0, 2, &choice))
            choice = 0;

        switch (choice)
        {
        case 1:
            runStudentPanel(items, &count, &admin, &dirty);
            break;
        case 2:
            runAdminPanel(items, &count, &admin, &dirty);
            break;
        case 0:
            break;
        }

        syncChanges(items, count, &admin, &dirty);
    } while (choice != 0);

    puts("Goodbye.");
    return EXIT_SUCCESS;
}