#ifndef MATCHING_H
#define MATCHING_H

#include "items.h"

typedef struct {
    int itemIndex;
    int score;
} MatchResult;

int calculateMatchScore(const Item *a, const Item *b);
void findMatches(const Item items[], int count);

#endif