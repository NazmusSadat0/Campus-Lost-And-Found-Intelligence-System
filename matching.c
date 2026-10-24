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
        score += 30;
    }
    if (equalText(a->name, b->name) || containsText(a->name, b->name) ||
        containsText(b->name, a->name))
    {
        score += 25;
    }
    if (equalText(a->colour, b->colour) || containsText(a->colour, b->colour) ||
        containsText(b->colour, a->colour))
    {
        score += 15;
    }
    if (a->location == b->location)
    {
        score += 15;
    }
    if (equalText(a->date, b->date) ||
        containsText(a->description, b->description) ||
        containsText(b->description, a->description))
    {
        score += 15;
    }

    return score;
}

static void printRankedMatches(
    const Item items[],
    int count,
    const Item *queryItem)
{
    int matches = 0;
    MatchResult *results = NULL;

    if (count <= 0)
    {
        puts("No reports available in the system.");
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
        int score = calculateMatchScore(queryItem, &items[i]);
        if (score >= 50)
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

    printf("\nResult: %d possible match(es) found (minimum score 50/100).\n", matches);

    for (int i = 0; i < matches; i++)
    {
        const Item *p = &items[results[i].itemIndex];
        int s = results[i].score;
        showItem(p);
        printf("Your Score: %d/100 (%s)\n",
               s, s >= 75 ? "Strong Ownership/Match Confidence" : "Partial Match");
        printf("Matched Answers:");
        if (queryItem->category == p->category)
            printf(" [Category +30]");
        if (equalText(queryItem->name, p->name) ||
            containsText(queryItem->name, p->name) ||
            containsText(p->name, queryItem->name))
            printf(" [Name +25]");
        if (equalText(queryItem->colour, p->colour) ||
            containsText(queryItem->colour, p->colour) ||
            containsText(p->colour, queryItem->colour))
            printf(" [Colour +15]");
        if (queryItem->location == p->location)
            printf(" [Location +15]");
        if (equalText(queryItem->date, p->date) ||
            containsText(queryItem->description, p->description) ||
            containsText(p->description, queryItem->description))
        {
            printf(" [Description/Date Clue +15]");
        }
        puts("");
    }

    free(results);
}

void findMatches(const Item items[], int count)
{
    int mode;
    puts("\n--- INTELLIGENT MATCHING & QUESTIONNAIRE ---");
    puts("1. Answer Questions to Find / Verify an Item (Scored out of 100)");
    puts("2. Match Using an Existing Report ID");

    if (!readNumber("Choice: ", 1, 2, &mode))
    {
        return;
    }

    if (mode == 1)
    {
        Item answer = {0};
        int searchFor;

        puts("\nAnswer the following 6 questions to calculate your match score:");
        if (!readNumber(
                "Q1. Did you (1) Lose an item [search Found list] or "
                "(2) Find an item [search Lost list]? ",
                1, 2, &searchFor))
        {
            return;
        }
        answer.type = (searchFor == 1) ? LOST : FOUND;
        answer.status = OPEN;

        if (!chooseOption("Q2. Select item category (+30 pts):",
                          categories, CATEGORY_COUNT, &answer.category) ||
            !readText("Q3. What is the item name? (+25 pts): ",
                      answer.name, sizeof answer.name) ||
            !readText("Q4. What colour is the item? (+15 pts): ",
                      answer.colour, sizeof answer.colour) ||
            !chooseOption("Q5. Where was it lost/found? (+15 pts):",
                          locations, LOCATION_COUNT, &answer.location) ||
            !readText("Q6. Enter a specific clue/detail (e.g. case, roll, tag) (+15 pts): ",
                      answer.description, sizeof answer.description))
        {
            return;
        }

        printRankedMatches(items, count, &answer);
        return;
    }

    int id, index;
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
    if (items[index].status != OPEN)
    {
        puts("Only open reports can be matched.");
        return;
    }

    printRankedMatches(items, count, &items[index]);
}