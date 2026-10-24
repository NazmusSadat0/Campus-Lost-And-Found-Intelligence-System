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

static int askVerificationQuestions(Item *answer)
{
    int searchFor;

    puts("\nAnswer these 6 verification questions (Minimum 60/100 required to claim):");
    if (!readNumber(
            "Q1. Did you (1) Lose an item [check Found items] or "
            "(2) Find an item [check Lost items]? ",
            1, 2, &searchFor))
    {
        return 0;
    }
    answer->type = (searchFor == 1) ? LOST : FOUND;
    answer->status = OPEN;

    if (!chooseOption("Q2. Select item category (+30 pts):",
                      categories, CATEGORY_COUNT, &answer->category) ||
        !readText("Q3. What is the item name? (+25 pts): ",
                  answer->name, sizeof answer->name) ||
        !readText("Q4. What colour is the item? (+15 pts): ",
                  answer->colour, sizeof answer->colour) ||
        !chooseOption("Q5. Where was it lost/found? (+15 pts):",
                      locations, LOCATION_COUNT, &answer->location) ||
        !readText("Q6. Enter a specific clue/detail (e.g. case, roll, tag) (+15 pts): ",
                  answer->description, sizeof answer->description))
    {
        return 0;
    }

    return 1;
}

static int completeClaimStep(
    Item items[],
    int count,
    const MatchResult results[],
    int matches)
{
    int chosenIndex = -1;
    int yes;

    puts("\n--- PROCEEDING DIRECTLY TO CLAIM / HANDOVER ---");
    if (matches == 1)
    {
        chosenIndex = results[0].itemIndex;
        printf("Matched Report #%d (%s) selected for claim.\n",
               items[chosenIndex].id, items[chosenIndex].name);
    }
    else
    {
        int id;
        if (!readNumber("Enter the matched Report ID from above to claim (0 to cancel): ",
                        0, 9999, &id) ||
            id == 0)
        {
            puts("Claim skipped.");
            return 0;
        }
        for (int i = 0; i < matches; i++)
        {
            if (items[results[i].itemIndex].id == id)
            {
                chosenIndex = results[i].itemIndex;
                break;
            }
        }
        if (chosenIndex < 0)
        {
            puts("You can only claim a Report ID that passed your verification quiz.");
            return 0;
        }
    }

    if (!readNumber("Confirm claiming and resolving this item now? (1 Yes, 0 No): ",
                    0, 1, &yes) ||
        !yes)
    {
        puts("Claim cancelled.");
        return 0;
    }

    items[chosenIndex].status = RESOLVED;
    printf("Success! Report #%d (%s) has been claimed and marked as Resolved.\n",
           items[chosenIndex].id, items[chosenIndex].name);
    (void)count;
    return 1;
}

int findMatches(Item items[], int count)
{
    Item answer = {0};
    int matches = 0;
    int claimed = 0;
    MatchResult *results = NULL;

    puts("\n--- INTELLIGENT QUESTIONNAIRE & MATCH SCORING ---");
    if (count <= 0)
    {
        puts("No reports available in the system.");
        return 0;
    }

    if (!askVerificationQuestions(&answer))
    {
        return 0;
    }

    /* Dynamic memory allocation (malloc) for storing candidate matches */
    results = (MatchResult *)malloc((size_t)count * sizeof(MatchResult));
    if (!results)
    {
        fprintf(stderr, "Memory allocation failed for match results.\n");
        return 0;
    }

    for (int i = 0; i < count; i++)
    {
        int score = calculateMatchScore(&answer, &items[i]);
        if (score >= MIN_CLAIM_SCORE)
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

    if (matches == 0)
    {
        puts("\nResult: 0 qualifying matches found (Minimum 60/100 required).");
        puts("Your answers did not reach the verification threshold to view or claim an item.");
        free(results);
        return 0;
    }

    printf("\nVerified! %d matching report(s) scored >= %d/100:\n",
           matches, MIN_CLAIM_SCORE);

    for (int i = 0; i < matches; i++)
    {
        const Item *p = &items[results[i].itemIndex];
        int s = results[i].score;
        showItem(p);
        printf("Your Score: %d/100 (Qualified to Claim)\n", s);
        printf("Matched Answers:");
        if (answer.category == p->category)
            printf(" [Category +30]");
        if (equalText(answer.name, p->name) ||
            containsText(answer.name, p->name) ||
            containsText(p->name, answer.name))
            printf(" [Name +25]");
        if (equalText(answer.colour, p->colour) ||
            containsText(answer.colour, p->colour) ||
            containsText(p->colour, answer.colour))
            printf(" [Colour +15]");
        if (answer.location == p->location)
            printf(" [Location +15]");
        if (equalText(answer.date, p->date) ||
            containsText(answer.description, p->description) ||
            containsText(p->description, answer.description))
        {
            printf(" [Clue +15]");
        }
        puts("");
    }

    claimed = completeClaimStep(items, count, results, matches);
    free(results);
    return claimed;
}

int claimItemByQuestions(Item items[], int count)
{
    puts("\n--- DIRECT ITEM CLAIM (OWNERSHIP VERIFICATION REQUIRED) ---");
    puts("To prevent false claims, answer the verification questions first:");
    return findMatches(items, count);
}