#include "items.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *categories[CATEGORY_COUNT] = {
    "Electronics",
    "ID / cards",
    "Books / stationery",
    "Keys",
    "Clothing",
    "Other"
};

const char *locations[LOCATION_COUNT] = {
    "Central Library",
    "Academic Building",
    "Cafeteria",
    "Residential Hall",
    "Playground",
    "Campus Gate",
    "Other"
};

/*
 * Reads a complete line.
 * Rejects empty or oversized input.
 * Removes whitespace from the beginning and end.
 * Returns 0 when input ends.
 */

int readText(const char *prompt, char *output, size_t size) {
    char line[512];

    for (;;) {
        size_t start = 0;
        size_t end;
        size_t i;
        int ch;

        printf("%s", prompt);
        fflush(stdout);

        if (!fgets(line, sizeof line, stdin)) {
            return 0;
        }

        if (!strchr(line, '\n') && !feof(stdin)) {
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }

            puts("Input too long. Please try again.");
            continue;
        }

        end = strlen(line);

        while (start < end &&
               isspace((unsigned char)line[start])) {
            start++;
        }

        while (end > start &&
               isspace((unsigned char)line[end - 1])) {
            end--;
        }

        if (end == start) {
            puts("This field cannot be empty.");
            continue;
        }

        if (end - start >= size) {
            printf("Use at most %zu characters.\n", size - 1);
            continue;
        }

        for (i = start; i < end; i++) {
            if (iscntrl((unsigned char)line[i])) {
                break;
            }
        }

        if (i != end) {
            puts("Control characters are not allowed.");
            continue;
        }

        memcpy(output, line + start, end - start);
        output[end - start] = '\0';

        return 1;
    }
}

/* Reads and validates a whole number within a range. */
int readNumber(
    const char *prompt,
    int min,
    int max,
    int *result
) {
    char text[40];
    char *end;
    long value;

    while (readText(prompt, text, sizeof text)) {
        errno = 0;
        value = strtol(text, &end, 10);

        if (!errno &&
            *end == '\0' &&
            value >= min &&
            value <= max) {

            *result = (int)value;
            return 1;
        }

        printf(
            "Enter a whole number from %d to %d.\n",
            min,
            max
        );
    }

    return 0;
}

/* Checks YYYY-MM-DD format and real calendar dates. */
int validDate(const char *date) {
    int year;
    int month;
    int day;

    int days[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (strlen(date) != 10 ||
        date[4] != '-' ||
        date[7] != '-') {
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        if (i != 4 &&
            i != 7 &&
            !isdigit((unsigned char)date[i])) {
            return 0;
        }
    }

    if (sscanf(date, "%4d-%2d-%2d",
               &year, &month, &day) != 3) {
        return 0;
    }

    if (year < 1900 || month < 1 || month > 12) {
        return 0;
    }

    if (year % 400 == 0 ||
        (year % 4 == 0 && year % 100 != 0)) {
        days[1] = 29;
    }

    return day >= 1 && day <= days[month - 1];
}

/* Compares nonempty strings without ASCII case differences. */
int equalText(const char *a, const char *b) {
    if (!*a || !*b) {
        return 0;
    }

    while (*a && *b) {
        if (tolower((unsigned char)*a) !=
            tolower((unsigned char)*b)) {
            return 0;
        }

        a++;
        b++;
    }

    return *a == *b;
}

/* Searches text without ASCII case differences. */

int containsText(const char *text, const char *query) {
    char lowerText[200];
    char lowerQuery[200];
    size_t i;

    if (strlen(text) >= sizeof lowerText ||
        strlen(query) >= sizeof lowerQuery) {
        return 0;
    }

    for (i = 0; text[i]; i++) {
        lowerText[i] =
            (char)tolower((unsigned char)text[i]);
    }

    lowerText[i] = '\0';

    for (i = 0; query[i]; i++) {
        lowerQuery[i] =
            (char)tolower((unsigned char)query[i]);
    }

    lowerQuery[i] = '\0';

    return *query &&
           strstr(lowerText, lowerQuery) != NULL;
}

/* Demonstrates pointer arithmetic while searching the array. */
int findIndex(const Item items[], int count, int id) {
    for (const Item *p = items; p < items + count; p++) {
        if (p->id == id) {
            return (int)(p - items);
        }
    }

    return -1;
}

/* Checks strings read from the binary data file. */
static int validString(const char *s, size_t size) {
    const char *end = memchr(s, '\0', size);

    if (!end || end == s) {
        return 0;
    }

    for (const char *p = s; p < end; p++) {
        if (iscntrl((unsigned char)*p)) {
            return 0;
        }
    }

    return 1;
}

int validItem(const Item *p) {
    return
        p->id >= 1000 &&
        p->id <= 9999 &&

        (p->type == LOST || p->type == FOUND) &&
        (p->status == OPEN || p->status == RESOLVED) &&

        p->category >= 1 &&
        p->category <= CATEGORY_COUNT &&

        p->location >= 1 &&
        p->location <= LOCATION_COUNT &&

        validString(p->name, sizeof p->name) &&
        validString(p->colour, sizeof p->colour) &&
        validString(p->date, sizeof p->date) &&
        validDate(p->date) &&

        validString(
            p->description,
            sizeof p->description
        ) &&

        validString(p->reporter, sizeof p->reporter) &&
        validString(p->contact, sizeof p->contact);
}

/* Displays a numbered list and reads the selection. */
static int choose(
    const char *title,
    const char *options[],
    int count,
    int *out
) {
    puts(title);

    for (int i = 0; i < count; i++) {
        printf("  %d. %s\n", i + 1, options[i]);
    }

    return readNumber("Choice: ", 1, count, out);
}

/* Collects the details of a new report. */
static int enterDetails(Item *p) {
    if (!readText(
            "Item name: ",
            p->name,
            sizeof p->name
        )) {
        return 0;
    }

    if (!choose(
            "Category",
            categories,
            CATEGORY_COUNT,
            &p->category
        )) {
        return 0;
    }

    if (!readText(
            "Colour: ",
            p->colour,
            sizeof p->colour
        )) {
        return 0;
    }

    if (!choose(
            "Location (put exact place in description)",
            locations,
            LOCATION_COUNT,
            &p->location
        )) {
        return 0;
    }

    do {
        if (!readText(
                "Date lost/found (YYYY-MM-DD): ",
                p->date,
                sizeof p->date
            )) {
            return 0;
        }

        if (validDate(p->date)) {
            break;
        }

        puts("Enter a real calendar date, year 1900 or later.");

    } while (1);

    return
        readText(
            "Description (keep private ownership clues out): ",
            p->description,
            sizeof p->description
        ) &&
        readText(
            "Reporter name: ",
            p->reporter,
            sizeof p->reporter
        ) &&
        readText(
            "Contact: ",
            p->contact,
            sizeof p->contact
        );
}

void showItem(const Item *p) {
    printf(
        "\nReport #%d | %s | %s\n",
        p->id,
        p->type == LOST ? "Lost" : "Found",
        p->status == OPEN ? "Open" : "Resolved"
    );

    printf("Item: %s\n", p->name);
    printf("Category: %s\n", categories[p->category - 1]);
    printf("Colour: %s\n", p->colour);
    printf("Location: %s\n", locations[p->location - 1]);
    printf("Date: %s\n", p->date);
    printf("Description: %s\n", p->description);
    printf("Reporter: %s\n", p->reporter);
    printf("Contact: %s\n", p->contact);
}

static void showRow(const Item *p) {
    printf(
        "%-6d %-6s %-24.24s %-20.20s %s\n",
        p->id,
        p->type == LOST ? "Lost" : "Found",
        p->name,
        locations[p->location - 1],
        p->status == OPEN ? "Open" : "Resolved"
    );
}

void displayItems(const Item items[], int count) {
    int filter;
    int shown = 0;

    puts("1. All  2. Lost  3. Found  4. Open  5. Resolved");

    if (!readNumber("Filter: ", 1, 5, &filter)) {
        return;
    }

    printf(
        "%-6s %-6s %-24s %-20s %s\n",
        "ID", "Type", "Item", "Location", "Status"
    );

    for (int i = 0; i < count; i++) {
        const Item *p = &items[i];

        if (filter == 1 ||
            (filter == 2 && p->type == LOST) ||
            (filter == 3 && p->type == FOUND) ||
            (filter == 4 && p->status == OPEN) ||
            (filter == 5 && p->status == RESOLVED)) {

            showRow(p);
            shown++;
        }
    }

    printf(
        "%d report(s). Use Search by ID for full details.\n",
        shown
    );
}

void searchItems(const Item items[], int count) {
    int mode;
    int value = 0;
    int shown = 0;
    char query[80] = "";

    puts("1. ID");
    puts("2. Name / description keyword");
    puts("3. Category");
    puts("4. Location");

    if (!readNumber("Search by: ", 1, 4, &mode)) {
        return;
    }

    if (mode == 1 &&
        !readNumber("Report ID: ", 1000, 9999, &value)) {
        return;
    }

    if (mode == 2 &&
        !readText("Keyword: ", query, sizeof query)) {
        return;
    }

    if (mode == 3 &&
        !choose(
            "Category",
            categories,
            CATEGORY_COUNT,
            &value
        )) {
        return;
    }

    if (mode == 4 &&
        !choose(
            "Location",
            locations,
            LOCATION_COUNT,
            &value
        )) {
        return;
    }

    for (int i = 0; i < count; i++) {
        const Item *p = &items[i];

        if ((mode == 1 && p->id == value) ||

            (mode == 2 &&
             (containsText(p->name, query) ||
              containsText(p->description, query))) ||

            (mode == 3 && p->category == value) ||

            (mode == 4 && p->location == value)) {

            showItem(p);
            shown++;
        }
    }

    printf("%d result(s).\n", shown);
}

int addItem(Item items[], int *count, int type) {
    Item item = {0};

    if (*count >= MAX_ITEMS) {
        puts("Record capacity reached.");
        return 0;
    }

    item.type = type;
    item.status = OPEN;

    if (!enterDetails(&item)) {
        puts("Report cancelled.");
        return 0;
    }

    /*
     * Start with a random four-digit ID.
     * If already used, advance until a free ID is found.
     */
    item.id = 1000 + rand() % 9000;

    while (findIndex(items, *count, item.id) != -1) {
        item.id = item.id == 9999 ? 1000 : item.id + 1;
    }

    items[*count] = item;
    (*count)++;

    printf("Added report #%d.\n", item.id);

    return 1;
}

/* Reads an ID and returns the record's array index. */
static int selectIndex(const Item items[], int count) {
    int id;
    int index;

    if (!readNumber("Report ID: ", 1000, 9999, &id)) {
        return -1;
    }

    index = findIndex(items, count, id);

    if (index < 0) {
        puts("Report not found.");
    }

    return index;
}

int updateItem(Item items[], int count) {
    int index = selectIndex(items, count);
    int field;

    if (index < 0) {
        return 0;
    }

    /* Edit a copy first, then commit the completed change. */
    Item copy = items[index];

    showItem(&copy);

    puts("1. Name");
    puts("2. Category");
    puts("3. Colour");
    puts("4. Location");
    puts("5. Date");
    puts("6. Description");
    puts("7. Reporter");
    puts("8. Contact");
    puts("9. Status");
    puts("10. Lost/found type");

    if (!readNumber("Field: ", 1, 10, &field)) {
        return 0;
    }

    switch (field) {
        case 1:
            if (!readText(
                    "New name: ",
                    copy.name,
                    sizeof copy.name
                )) {
                return 0;
            }
            break;

        case 2:
            if (!choose(
                    "Category",
                    categories,
                    CATEGORY_COUNT,
                    &copy.category
                )) {
                return 0;
            }
            break;

        case 3:
            if (!readText(
                    "New colour: ",
                    copy.colour,
                    sizeof copy.colour
                )) {
                return 0;
            }
            break;

        case 4:
            if (!choose(
                    "Location",
                    locations,
                    LOCATION_COUNT,
                    &copy.location
                )) {
                return 0;
            }
            break;

        case 5:
            do {
                if (!readText(
                        "New date (YYYY-MM-DD): ",
                        copy.date,
                        sizeof copy.date
                    )) {
                    return 0;
                }

                if (validDate(copy.date)) {
                    break;
                }

                puts("Invalid calendar date.");

            } while (1);
            break;

        case 6:
            if (!readText(
                    "New description: ",
                    copy.description,
                    sizeof copy.description
                )) {
                return 0;
            }
            break;

        case 7:
            if (!readText(
                    "New reporter: ",
                    copy.reporter,
                    sizeof copy.reporter
                )) {
                return 0;
            }
            break;

        case 8:
            if (!readText(
                    "New contact: ",
                    copy.contact,
                    sizeof copy.contact
                )) {
                return 0;
            }
            break;

        case 9:
            if (!readNumber(
                    "Status (1 Open, 2 Resolved): ",
                    1,
                    2,
                    &copy.status
                )) {
                return 0;
            }
            break;

        case 10:
            if (!readNumber(
                    "Type (1 Lost, 2 Found): ",
                    1,
                    2,
                    &copy.type
                )) {
                return 0;
            }
            break;
    }

    items[index] = copy;

    puts("Report updated.");
    return 1;
}

int deleteItem(Item items[], int *count) {
    int index = selectIndex(items, *count);
    int yes;

    if (index < 0) {
        return 0;
    }

    showItem(&items[index]);

    if (!readNumber(
            "Delete permanently? (1 Yes, 0 No): ",
            0,
            1,
            &yes
        ) || !yes) {
        return 0;
    }

    /* Shift later records left to fill the removed position. */
    for (int i = index; i < *count - 1; i++) {
        items[i] = items[i + 1];
    }

    (*count)--;

    puts("Report deleted.");
    return 1;
}

int recordReturn(Item items[], int count) {
    int index = selectIndex(items, count);
    int otherID;
    int other = -1;
    int yes;

    if (index < 0) {
        return 0;
    }

    if (items[index].status != OPEN) {
        puts("Report is already resolved.");
        return 0;
    }

    showItem(&items[index]);

    if (!readNumber(
            "Opposite lost/found report ID (0 if none): ",
            0,
            9999,
            &otherID
        )) {
        return 0;
    }

    if (otherID) {
        other = findIndex(items, count, otherID);

        if (other < 0 ||
            items[other].type == items[index].type ||
            items[other].status != OPEN) {

            puts(
                "Related report must exist, be open, "
                "and have the opposite type."
            );

            return 0;
        }

        showItem(&items[other]);
    }

    if (!readNumber(
            "Ownership verified AND item handed over? "
            "(1 Yes, 0 No): ",
            0,
            1,
            &yes
        ) || !yes) {
        return 0;
    }

    items[index].status = RESOLVED;

    if (other >= 0) {
        items[other].status = RESOLVED;
    }

    puts("Return recorded; selected report(s) resolved.");

    return 1;
}