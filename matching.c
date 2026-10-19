#include "matching.h"

#include <stdio.h>
#include <stdlib.h>

int calculateMatchScore(const Item *a, const Item *b)
{
    int score = 0;

    /* Only compare open reports of opposite types. */
    if (a->type == b->type || a->status != OPEN || b->status != OPEN)
    {
        return 0;
    }

    if (a->category == b->category)
    {
        score += 40;
    }
    if (equalText(a->name, b->name) || containsText(a->name, b->name) ||
        containsText(b->name, a->name))
    {
        score += 30;
    }
    if (equalText(a->colour, b->colour))
    {
        score += 15;
    }
    if (a->location == b->location)
    {
        score += 15;
    }

    return score;
}

void findMatches(const Item items[], int count)
{
    int id, index, matches = 0;
    MatchResult *results = NULL;

    if (!readNumber("Report ID to match: ", 1000, 9999, &id))
    {
        return;
    }

    index = findIndex(items, count, id);
    if (index < 0 || items[index].status == BLACKLISTED)
    {
        puts("Report not found.");
        return;
    }

    const Item *target = &items[index];
    if (target->status != OPEN)
    {
        puts("Only open reports can be matched.");
        return;
    }

    if (count <= 0)
    {
        puts("No other reports available.");
        return;
    }

    /* Dynamic memory allocation (malloc) for storing candidate matches */
    results = (MatchResult *)malloc((size_t)count * sizeof(MatchResult));
    if (!results)
    {
        fprintf(stderr, "Memory allocation failed for match results.\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        int score = calculateMatchScore(target, &items[i]);
        if (score >= 55)
        {
            results[matches].itemIndex = i;
            results[matches].score = score;
            matches++;
        }
    }

    /* Insertion sort in descending score order */
    for (int i = 1; i < matches; i++)
    {
        MatchResult key = results[i];
        int j = i - 1;
        while (j >= 0 && results[j].score < key.score)
        {
            results[j + 1] = results[j];
            j--;
        }
        results[j + 1] = key;
    }

    printf("%d possible match(es), minimum score 55/100.\n", matches);

    for (int i = 0; i < matches; i++)
    {
        const Item *p = &items[results[i].itemIndex];
        showItem(p);
        printf("Match score: %d/100\n", results[i].score);
        printf("Matched fields:");
        if (target->category == p->category)
            printf(" category");
        if (equalText(target->name, p->name) ||
            containsText(target->name, p->name) ||
            containsText(p->name, target->name))
            printf(" name");
        if (equalText(target->colour, p->colour))
            printf(" colour");
        if (target->location == p->location)
            printf(" location");
        puts("");
    }

    puts("Scores rank similar details; they are not ownership proof.");
    free(results);
}