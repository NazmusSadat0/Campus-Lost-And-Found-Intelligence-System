#include "items.h"

#include <stdio.h>

int calculateMatchScore(const Item *a, const Item *b) {
    int score = 0;

    /* Only compare open reports of opposite types. */
    if (a->type == b->type ||
        a->status != OPEN ||
        b->status != OPEN) {
        return 0;
    }

    if (a->category == b->category) {
        score += 40;
    }

    if (equalText(a->name, b->name)) {
        score += 30;
    }

    if (equalText(a->colour, b->colour)) {
        score += 15;
    }

    if (a->location == b->location) {
        score += 15;
    }

    return score;
}

void findMatches(const Item items[], int count) {
    int id;
    int index;
    int matches = 0;

    int indices[MAX_ITEMS];
    int scores[MAX_ITEMS];

    if (!readNumber(
            "Report ID to match: ",
            1000,
            9999,
            &id
        )) {
        return;
    }

    index = findIndex(items, count, id);

    if (index < 0) {
        puts("Report not found.");
        return;
    }

    const Item *target = &items[index];

    if (target->status != OPEN) {
        puts("Only open reports can be matched.");
        return;
    }

    for (int i = 0; i < count; i++) {
        int score = calculateMatchScore(target, &items[i]);

        if (score >= 55) {
            indices[matches] = i;
            scores[matches] = score;
            matches++;
        }
    }

    /*
     * Insertion sort in descending score order.
     * The original item array stays unchanged.
     */
    for (int i = 1; i < matches; i++) {
        int score = scores[i];
        int itemIndex = indices[i];
        int j = i - 1;

        while (j >= 0 && scores[j] < score) {
            scores[j + 1] = scores[j];
            indices[j + 1] = indices[j];
            j--;
        }

        scores[j + 1] = score;
        indices[j + 1] = itemIndex;
    }

    printf(
        "%d possible match(es), minimum score 55/100.\n",
        matches
    );

    for (int i = 0; i < matches; i++) {
        const Item *p = &items[indices[i]];

        showItem(p);

        printf("Match score: %d/100\n", scores[i]);
        printf("Matched fields:");

        if (target->category == p->category) {
            printf(" category");
        }

        if (equalText(target->name, p->name)) {
            printf(" name");
        }

        if (equalText(target->colour, p->colour)) {
            printf(" colour");
        }

        if (target->location == p->location) {
            printf(" location");
        }

        puts("");
    }

    puts(
        "Scores rank similar details; "
        "they are not ownership proof or probabilities."
    );
}