#include "items.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    static Item items[MAX_ITEMS];

    int count = 0;
    int choice;
    int changed;
    int dirty = 0;

    srand((unsigned int)time(NULL));

    if (!loadItems(DATA_FILE, items, &count)) {
        return EXIT_FAILURE;
    }

    printf("Loaded %d report(s).\n", count);

    do {
        puts("\nCAMPUS LOST & FOUND INTELLIGENCE SYSTEM");
        puts("-------------------------------------");
        puts("1. Report Lost Item");
        puts("2. Report Found Item");
        puts("3. Display Reports");
        puts("4. Search Reports");
        puts("5. Update Report");
        puts("6. Delete Report");
        puts("7. Find Possible Matches");
        puts("8. Record Item Return");
        puts("0. Save and Exit");

        if (!readNumber("Choice: ", 0, 8, &choice)) {
            choice = 0;
        }

        changed = 0;

        switch (choice) {
            case 1:
                changed = addItem(items, &count, LOST);
                break;

            case 2:
                changed = addItem(items, &count, FOUND);
                break;

            case 3:
                displayItems(items, count);
                break;

            case 4:
                searchItems(items, count);
                break;

            case 5:
                changed = updateItem(items, count);
                break;

            case 6:
                changed = deleteItem(items, &count);
                break;

            case 7:
                findMatches(items, count);
                break;

            case 8:
                changed = recordReturn(items, count);
                break;

            case 0:
                break;
        }

        if (changed) {
            dirty = 1;
        }

        if (dirty) {
            if (saveItems(DATA_FILE, items, count)) {
                dirty = 0;
                puts("Changes saved.");
            } else {
                fprintf(
                    stderr,
                    "Changes are NOT saved. Keep this session open "
                    "and fix file permissions or available space.\n"
                );
            }
        }

        if (choice == 0 && dirty) {
            if (feof(stdin) || ferror(stdin)) {
                return EXIT_FAILURE;
            }

            puts("Exit cancelled because saving failed.");
            puts("You can retry with option 0.");
            choice = -1;
        }

    } while (choice != 0);

    puts("Goodbye.");

    return EXIT_SUCCESS;
}