#include "admin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static AdminData *activeAdmin = NULL;

void setActiveAdminData(AdminData *admin)
{
    activeAdmin = admin;
}

void initDefaultAdminData(AdminData *admin)
{
    const char *defaults[] = {
        "weapon", "gun", "pistol", "rifle", "knife", "dagger",
        "bomb", "explosive", "drugs", "narcotics", "alcohol", "contraband"};

    int n = (int)(sizeof(defaults) / sizeof(defaults[0]));

    memset(admin, 0, sizeof(*admin));

    admin->kwCount = n;

    for (int i = 0; i < n; i++)
    {
        strcpy(admin->keywords[i], defaults[i]);
    }
    activeAdmin = admin;
}

const char *detectBlacklistedKeyword(const Item *item)
{
    if (!activeAdmin)
        return NULL;

    for (int i = 0; i < activeAdmin->kwCount; i++)
    {
        if (containsText(item->name, activeAdmin->keywords[i]) ||
            containsText(item->description, activeAdmin->keywords[i]))
        {
            return activeAdmin->keywords[i];
        }
    }
    return NULL;
}

int updateItem(Item items[], int count, const AdminData *admin)
{
    int index = selectIndex(items, count), field;

    if (index < 0)
        return 0;

    Item copy = items[index];
    showItem(&copy);

    puts("1. Name  2. Category  3. Colour  4. Location  5. Date");
    puts("6. Description  7. Reporter  8. Contact  9. Status  10. Type");

    if (!readNumber("Field: ", 1, 10, &field))
        return 0;
        
    switch (field)
    {
    case 1:
        if (!readText("New name: ", copy.name, sizeof copy.name))
            return 0;
        break;
    case 2:
        if (!chooseOption("Category", categories, CATEGORY_COUNT, &copy.category))
            return 0;
        break;
    case 3:
        if (!readText("New colour: ", copy.colour, sizeof copy.colour))
            return 0;
        break;
    case 4:
        if (!chooseOption("Location", locations, LOCATION_COUNT, &copy.location))
            return 0;
        break;
    case 5:
        for (;;)
        {
            if (!readText("New date (YYYY-MM-DD): ", copy.date, sizeof copy.date))
                return 0;
            if (validDate(copy.date))
                break;
            puts("Invalid calendar date.");
        }
        break;
    case 6:
        if (!readText("New description: ", copy.description, sizeof copy.description))
            return 0;
        break;
    case 7:
        if (!readText("New reporter: ", copy.reporter, sizeof copy.reporter))
            return 0;
        break;
    case 8:
        if (!readText("New contact: ", copy.contact, sizeof copy.contact))
            return 0;
        break;
    case 9:
        if (!readNumber("Status (1 Open, 2 Resolved, 3 Blacklisted): ", 1, 3, &copy.status))
            return 0;
        break;
    case 10:
        if (!readNumber("Type (1 Lost, 2 Found): ", 1, 2, &copy.type))
            return 0;
        break;
    }
    if (field == 1 || field == 6)
    {
        (void)admin;
        const char *kw = detectBlacklistedKeyword(&copy);
        if (kw)
        {
            copy.status = BLACKLISTED;
            printf("[ALERT] Blacklisted keyword (\"%s\") detected; status set to Blacklisted.\n", kw);
        }
    }
    items[index] = copy;
    puts("Report updated.");
    return 1;
}

int deleteItem(Item items[], int *count)
{
    int index = selectIndex(items, *count), yes;
    if (index < 0)
        return 0;
    showItem(&items[index]);
    if (!readNumber("Delete permanently? (1 Yes, 0 No): ", 0, 1, &yes) || !yes)
        return 0;
    for (int i = index; i < *count - 1; i++)
        items[i] = items[i + 1];
    (*count)--;
    puts("Report deleted.");
    return 1;
}

int reviewBlacklistedItems(Item items[], int *count)
{
    int found = 0, id, index, action;
    puts("\n--- BLACKLISTED / FLAGGED ITEM REPORTS ---");
    for (int i = 0; i < *count; i++)
    {
        if (items[i].status == BLACKLISTED)
        {
            showItem(&items[i]);
            found++;
        }
    }
    if (!found)
    {
        puts("No blacklisted item reports pending review.");
        return 0;
    }
    if (!readNumber("Enter Report ID to review (0 to cancel): ", 0, 9999, &id) || id == 0)
        return 0;
    index = findIndex(items, *count, id);
    if (index < 0 || items[index].status != BLACKLISTED)
    {
        puts("Blacklisted report not found.");
        return 0;
    }
    puts("1. Approve & Unflag (Mark as Open)");
    puts("2. Delete / Confiscate Report Permanently");
    puts("0. Cancel");
    if (!readNumber("Action: ", 0, 2, &action) || action == 0)
        return 0;
    if (action == 1)
    {
        items[index].status = OPEN;
        puts("Report approved and moved to Open status.");
        return 1;
    }
    for (int i = index; i < *count - 1; i++)
        items[i] = items[i + 1];
    (*count)--;
    puts("Blacklisted report permanently removed.");
    return 1;
}

int manageBlacklist(AdminData *admin, Item items[], int count)
{
    int action;
    puts("\n--- BLACKLIST KEYWORD MANAGEMENT ---");
    for (int i = 0; i < admin->kwCount; i++)
    {
        printf("  %d. %s\n", i + 1, admin->keywords[i]);
    }
    puts("1. Add prohibited keyword  2. Remove keyword  0. Back");
    if (!readNumber("Choice: ", 0, 2, &action) || action == 0)
        return 0;
    if (action == 1)
    {
        char kw[40];
        if (admin->kwCount >= MAX_KEYWORDS)
        {
            puts("Keyword list is full.");
            return 0;
        }
        if (!readText("New prohibited keyword: ", kw, sizeof kw))
            return 0;
        for (int i = 0; i < admin->kwCount; i++)
        {
            if (equalText(admin->keywords[i], kw))
            {
                puts("Keyword already exists.");
                return 0;
            }
        }
        memcpy(admin->keywords[admin->kwCount], kw, sizeof kw);
        admin->kwCount++;
        for (int i = 0; i < count; i++)
        {
            if (items[i].status != BLACKLISTED && containsText(items[i].name, kw))
            {
                items[i].status = BLACKLISTED;
            }
        }
        puts("Keyword added to blacklist.");
        return 1;
    }
    if (admin->kwCount == 0)
        return 0;
    int idx;
    if (!readNumber("Keyword number to remove: ", 1, admin->kwCount, &idx))
        return 0;
    for (int i = idx - 1; i < admin->kwCount - 1; i++)
    {
        strcpy(admin->keywords[i], admin->keywords[i + 1]);
    }
    admin->kwCount--;
    puts("Keyword removed.");
    return 1;
}

int submitFraudAllegation(const Item items[], int count, AdminData *admin)
{
    FraudReport fr = {0};
    int index;
    if (admin->fraudCount >= MAX_FRAUDS)
    {
        puts("Fraud allegation storage is full.");
        return 0;
    }
    index = selectIndex(items, count);
    if (index < 0 || items[index].status == BLACKLISTED)
    {
        puts("Valid public report not found.");
        return 0;
    }
    showItem(&items[index]);
    fr.itemId = items[index].id;
    fr.status = FRAUD_PENDING;
    if (!readText("Suspected false claimant / user name: ", fr.suspectName, sizeof fr.suspectName) ||
        !readText("Your name: ", fr.accuserName, sizeof fr.accuserName) ||
        !readText("Your contact: ", fr.accuserContact, sizeof fr.accuserContact))
    {
        return 0;
    }
    for (;;)
    {
        if (!readText("Date of allegation (YYYY-MM-DD): ", fr.date, sizeof fr.date))
            return 0;
        if (validDate(fr.date))
            break;
        puts("Enter a valid calendar date.");
    }
    if (!readText("Reason / suspicious claim details: ", fr.reason, sizeof fr.reason))
        return 0;
    fr.id = 1000 + rand() % 9000;
    admin->frauds[admin->fraudCount++] = fr;
    printf("Fraud allegation #%d submitted and forwarded to the Admin.\n", fr.id);
    return 1;
}

int reviewFraudAllegations(Item items[], int count, AdminData *admin)
{
    int id, found = -1, action;
    if (admin->fraudCount == 0)
    {
        puts("No fraud allegations submitted yet.");
        return 0;
    }
    puts("\n--- FRAUD ALLEGATIONS / SUSPICIOUS CLAIMS ---");
    for (int i = 0; i < admin->fraudCount; i++)
    {
        const FraudReport *f = &admin->frauds[i];
        const char *st = f->status == FRAUD_PENDING ? "Pending" : (f->status == FRAUD_VERIFIED ? "Verified" : "Dismissed");
        printf("Allegation #%d | Item #%d | Status: %s | Suspect: %s | By: %s (%s)\n",
               f->id, f->itemId, st, f->suspectName, f->accuserName, f->date);
        printf("  Reason: %s\n", f->reason);
    }
    if (!readNumber("Enter Allegation ID to review (0 to cancel): ", 0, 9999, &id) || id == 0)
        return 0;
    for (int i = 0; i < admin->fraudCount; i++)
    {
        if (admin->frauds[i].id == id)
        {
            found = i;
            break;
        }
    }
    if (found < 0)
    {
        puts("Allegation ID not found.");
        return 0;
    }
    int itemIdx = findIndex(items, count, admin->frauds[found].itemId);
    if (itemIdx >= 0)
        showItem(&items[itemIdx]);
    puts("1. Verify Fraud (Mark Verified & Reopen Item to Open)");
    puts("2. Dismiss Allegation");
    puts("0. Cancel");
    if (!readNumber("Decision: ", 0, 2, &action) || action == 0)
        return 0;
    if (action == 1)
    {
        admin->frauds[found].status = FRAUD_VERIFIED;
        if (itemIdx >= 0 && items[itemIdx].status == RESOLVED)
        {
            items[itemIdx].status = OPEN;
            puts("Fraud verified: related item report reopened to Open status.");
        }
        else
        {
            puts("Fraud allegation marked as Verified.");
        }
    }
    else
    {
        admin->frauds[found].status = FRAUD_DISMISSED;
        puts("Fraud allegation dismissed.");
    }
    return 1;
}