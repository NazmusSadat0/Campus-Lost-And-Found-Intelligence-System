#ifndef ITEMS_H
#define ITEMS_H

#include <stddef.h>

#define MAX_ITEMS 500
#define CATEGORY_COUNT 6
#define LOCATION_COUNT 7
#define DATA_FILE "items.dat"

#define LOST 1
#define FOUND 2

#define OPEN 1
#define RESOLVED 2

typedef struct
{
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
int validItem(const Item *item);

/* String processing */
int equalText(const char *a, const char *b);
int containsText(const char *text, const char *query);

/* Item operations */
int findIndex(const Item items[], int count, int id);
void showItem(const Item *item);
void displayItems(const Item items[], int count);
void searchItems(const Item items[], int count);

int addItem(Item items[], int *count, int type);
int updateItem(Item items[], int count);
int deleteItem(Item items[], int *count);
int recordReturn(Item items[], int count);

/* Matching */
int calculateMatchScore(const Item *a, const Item *b);
void findMatches(const Item items[], int count);

/* File handling */
int saveItems(const char *path, const Item items[], int count);
int loadItems(const char *path, Item items[], int *count);

#endif