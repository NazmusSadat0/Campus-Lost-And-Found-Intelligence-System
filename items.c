#include "items.h"
#include "admin.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *categories[CATEGORY_COUNT] = {
    "Electronics", "ID / cards", "Books / stationery",
    "Keys", "Clothing", "Other"};

const char *locations[LOCATION_COUNT] = {
    "Central Library", "Academic Building", "Cafeteria",
    "Residential Hall", "Playground", "Campus Gate", "Other"};

int readText(const char *prompt, char *output, size_t size)
{
    char line[512];
    for (;;)
    {
        size_t start = 0, end, i;
        int ch;
        printf("%s", prompt);
        fflush(stdout);
        if (!fgets(line, sizeof line, stdin))
            return 0;
        if (!strchr(line, '\n') && !feof(stdin))
        {
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }
            puts("Input too long. Please try again.");
            continue;
        }
        end = strlen(line);
        while (start < end && isspace((unsigned char)line[start]))
            start++;
        while (end > start && isspace((unsigned char)line[end - 1]))
            end--;
        if (end == start)
        {
            puts("This field cannot be empty.");
            continue;
        }
        if (end - start >= size)
        {
            printf("Use at most %zu characters.\n", size - 1);
            continue;
        }
        for (i = start; i < end; i++)
        {
            if (iscntrl((unsigned char)line[i]))
                break;
        }
        if (i != end)
        {
            puts("Control characters are not allowed.");
            continue;
        }
        memcpy(output, line + start, end - start);
        output[end - start] = '\0';
        return 1;
    }
}

int readNumber(const char *prompt, int min, int max, int *result)
{
    char text[40], *end;
    long value;
    while (readText(prompt, text, sizeof text))
    {
        errno = 0;
        value = strtol(text, &end, 10);
        if (!errno && *end == '\0' && value >= min && value <= max)
        {
            *result = (int)value;
            return 1;
        }
        printf("Enter a whole number from %d to %d.\n", min, max);
    }
    return 0;
}

int validDate(const char *date)
{
    int year, month, day;
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-')
        return 0;
    for (int i = 0; i < 10; i++)
    {
        if (i != 4 && i != 7 && !isdigit((unsigned char)date[i]))
            return 0;
    }
    if (sscanf(date, "%4d-%2d-%2d", &year, &month, &day) != 3)
        return 0;
    if (year < 1900 || month < 1 || month > 12)
        return 0;
    if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
        days[1] = 29;
    return day >= 1 && day <= days[month - 1];
}

int equalText(const char *a, const char *b)
{
    if (!*a || !*b)
        return 0;
    while (*a && *b)
    {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }
    return *a == *b;
}

int containsText(const char *text, const char *query)
{
    char lowerText[256], lowerQuery[256];
    size_t i;
    if (strlen(text) >= sizeof lowerText || strlen(query) >= sizeof lowerQuery)
        return 0;
    for (i = 0; text[i]; i++)
        lowerText[i] = (char)tolower((unsigned char)text[i]);
    lowerText[i] = '\0';
    for (i = 0; query[i]; i++)
        lowerQuery[i] = (char)tolower((unsigned char)query[i]);
    lowerQuery[i] = '\0';
    return *query && strstr(lowerText, lowerQuery) != NULL;
}

int findIndex(const Item items[], int count, int id)
{
    for (const Item *p = items; p < items + count; p++)
    {
        if (p->id == id)
            return (int)(p - items);
    }
    return -1;
}

int selectIndex(const Item items[], int count)
{
    int id, index;
    if (!readNumber("Report ID: ", 1000, 9999, &id))
        return -1;
    index = findIndex(items, count, id);
    if (index < 0)
        puts("Report not found.");
    return index;
}

int validString(const char *s, size_t size)
{
    const char *end = memchr(s, '\0', size);
    if (!end || end == s)
        return 0;
    for (const char *p = s; p < end; p++)
    {
        if (iscntrl((unsigned char)*p))
            return 0;
    }
    return 1;
}

int validItem(const Item *p)
{
    return p->id >= 1000 && p->id <= 9999 &&
           (p->type == LOST || p->type == FOUND) &&
           (p->status == OPEN || p->status == RESOLVED || p->status == BLACKLISTED) &&
           p->category >= 1 && p->category <= CATEGORY_COUNT &&
           p->location >= 1 && p->location <= LOCATION_COUNT &&
           validString(p->name, sizeof p->name) &&
           validString(p->colour, sizeof p->colour) &&
           validString(p->date, sizeof p->date) && validDate(p->date) &&
           validString(p->description, sizeof p->description) &&
           validString(p->reporter, sizeof p->reporter) &&
           validString(p->contact, sizeof p->contact);
}

int chooseOption(const char *title, const char *options[], int count, int *out)
{
    puts(title);
    for (int i = 0; i < count; i++)
        printf("  %d. %s\n", i + 1, options[i]);
    return readNumber("Choice: ", 1, count, out);
}

const char *statusLabel(int status)
{
    if (status == OPEN)
        return "Open";
    if (status == RESOLVED)
        return "Resolved";
    return "Blacklisted";
}

void showItem(const Item *p)
{
    printf("\nReport #%d | %s | %s\n", p->id,
           p->type == LOST ? "Lost" : "Found", statusLabel(p->status));
    printf("Item: %s\nCategory: %s\nColour: %s\nLocation: %s\n",
           p->name, categories[p->category - 1], p->colour, locations[p->location - 1]);
    printf("Date: %s\nDescription: %s\nReporter: %s\nContact: %s\n",
           p->date, p->description, p->reporter, p->contact);
}

void displayItems(const Item items[], int count, int isAdmin)
{
    int filter, maxFilter = isAdmin ? 6 : 5, shown = 0;
    puts(isAdmin ? "1. All  2. Lost  3. Found  4. Open  5. Resolved  6. Blacklisted"
                 : "1. All Public  2. Lost  3. Found  4. Open  5. Resolved");
    if (!readNumber("Filter: ", 1, maxFilter, &filter))
        return;
    printf("%-6s %-6s %-24s %-20s %s\n", "ID", "Type", "Item", "Location", "Status");
    for (int i = 0; i < count; i++)
    {
        const Item *p = &items[i];
        if (!isAdmin && p->status == BLACKLISTED)
            continue;
        if (filter == 1 || (filter == 2 && p->type == LOST) ||
            (filter == 3 && p->type == FOUND) || (filter == 4 && p->status == OPEN) ||
            (filter == 5 && p->status == RESOLVED) || (filter == 6 && p->status == BLACKLISTED))
        {
            printf("%-6d %-6s %-24.24s %-20.20s %s\n", p->id,
                   p->type == LOST ? "Lost" : "Found", p->name,
                   locations[p->location - 1], statusLabel(p->status));
            shown++;
        }
    }
    printf("%d report(s). Use Search by ID for full details.\n", shown);
}

void searchItems(const Item items[], int count, int isAdmin)
{
    int mode, value = 0, shown = 0;
    char query[80] = "";
    puts("1. ID\n2. Name / description keyword\n3. Category\n4. Location");
    if (!readNumber("Search by: ", 1, 4, &mode))
        return;
    if (mode == 1 && !readNumber("Report ID: ", 1000, 9999, &value))
        return;
    if (mode == 2 && !readText("Keyword: ", query, sizeof query))
        return;
    if (mode == 3 && !chooseOption("Category", categories, CATEGORY_COUNT, &value))
        return;
    if (mode == 4 && !chooseOption("Location", locations, LOCATION_COUNT, &value))
        return;
    for (int i = 0; i < count; i++)
    {
        const Item *p = &items[i];
        if (!isAdmin && p->status == BLACKLISTED)
            continue;
        if ((mode == 1 && p->id == value) ||
            (mode == 2 && (containsText(p->name, query) || containsText(p->description, query))) ||
            (mode == 3 && p->category == value) || (mode == 4 && p->location == value))
        {
            showItem(p);
            shown++;
        }
    }
    printf("%d result(s).\n", shown);
}

int addItem(Item items[], int *count, int type)
{
    Item item = {0};
    const char *matchedKw = NULL;
    if (*count >= MAX_ITEMS)
    {
        puts("Record capacity reached.");
        return 0;
    }
    item.type = type;
    if (!readText("Item name: ", item.name, sizeof item.name) ||
        !chooseOption("Category", categories, CATEGORY_COUNT, &item.category) ||
        !readText("Colour: ", item.colour, sizeof item.colour) ||
        !chooseOption("Location (put exact place in description)", locations, LOCATION_COUNT, &item.location))
    {
        puts("Report cancelled.");
        return 0;
    }
    for (;;)
    {
        if (!readText("Date lost/found (YYYY-MM-DD): ", item.date, sizeof item.date))
            return 0;
        if (validDate(item.date))
            break;
        puts("Enter a real calendar date, year 1900 or later.");
    }
    if (!readText("Description (keep private ownership clues out): ", item.description, sizeof item.description) ||
        !readText("Reporter name: ", item.reporter, sizeof item.reporter) ||
        !readText("Contact: ", item.contact, sizeof item.contact))
    {
        puts("Report cancelled.");
        return 0;
    }
    item.id = 1000 + rand() % 9000;
    while (findIndex(items, *count, item.id) != -1)
        item.id = item.id == 9999 ? 1000 : item.id + 1;
    matchedKw = detectBlacklistedKeyword(&item);
    if (matchedKw)
    {
        item.status = BLACKLISTED;
        printf("[ALERT] Prohibited item keyword detected (\"%s\")!\n", matchedKw);
        printf("Report #%d has been flagged and forwarded to the Admin for review.\n", item.id);
    }
    else
    {
        item.status = OPEN;
        printf("Added report #%d.\n", item.id);
    }
    items[*count] = item;
    (*count)++;
    return 1;
}

int recordReturn(Item items[], int count)
{
    int index = selectIndex(items, count), otherID, other = -1, yes;
    if (index < 0)
        return 0;
    if (items[index].status != OPEN)
    {
        puts("Only open non-blacklisted reports can be resolved.");
        return 0;
    }
    showItem(&items[index]);
    if (!readNumber("Opposite lost/found report ID (0 if none): ", 0, 9999, &otherID))
        return 0;
    if (otherID)
    {
        other = findIndex(items, count, otherID);
        if (other < 0 || items[other].type == items[index].type || items[other].status != OPEN)
        {
            puts("Related report must exist, be open, and have the opposite type.");
            return 0;
        }
        showItem(&items[other]);
    }
    if (!readNumber("Ownership verified AND item handed over? (1 Yes, 0 No): ", 0, 1, &yes) || !yes)
        return 0;
    items[index].status = RESOLVED;
    if (other >= 0)
        items[other].status = RESOLVED;
    puts("Return recorded; selected report(s) resolved.");
    return 1;
}