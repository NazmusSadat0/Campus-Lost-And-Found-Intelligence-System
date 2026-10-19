#ifndef ITEMS_H
#define ITEMS_H

#include <stddef.h>

#define MAX_ITEMS 500
#define CATEGORY_COUNT 6
#define LOCATION_COUNT 7
#define DATA_FILE "items.txt"

#define LOST 1
#define FOUND 2

#define OPEN 1
#define RESOLVED 2
#define BLACKLISTED 3

typedef struct {
    int id;
    int type;
    int category;
    int location;
    int status;

    char name[80];
    char colour[30];
    char date[11];
    char description[200];
    char reporter[60];
    char contact[60];
} Item;

extern const char *categories[CATEGORY_COUNT];
extern const char *locations[LOCATION_COUNT];

/* Input and validation */
int readText(const char *prompt, char *output, size_t size);
int readNumber(const char *prompt, int min, int max, int *result);
int validDate(const char *date);
int validString(const char *s, size_t size);
int validItem(const Item *item);
int chooseOption(const char *title, const char *options[], int count, int *out);

/* String processing */
int equalText(const char *a, const char *b);
int containsText(const char *text, const char *query);

/* Core item operations */
int findIndex(const Item items[], int count, int id);
int selectIndex(const Item items[], int count);
const char *statusLabel(int status);
void showItem(const Item *item);
void displayItems(const Item items[], int count, int isAdmin);
void searchItems(const Item items[], int count, int isAdmin);
int addItem(Item items[], int *count, int type);
int recordReturn(Item items[], int count);

#endif