#ifndef MATCHING_H
#define MATCHING_H

#include "items.h"

#define MIN_CLAIM_SCORE 60

typedef struct {
    int itemIndex;
    int score;
} MatchResult;

int calculateMatchScore(const Item *a, const Item *b);
int findMatches(Item items[], int count);
int claimItemByQuestions(Item items[], int count);

#endif