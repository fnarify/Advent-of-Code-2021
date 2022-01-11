#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BASE     36
#define STEPS    40
#define MAX_IN   50
#define MAX_PAIR 200

struct Pair {
    char name[3];
    char out;
    long decomp[2];
};
typedef struct Pair Pair;

int contain(int *a, int n, int val)
{
    int ret = 0;
    for (int i = 0; i < n; i++)
    {
        if (a[i] == val) {ret = 1;}
    }
    return ret;
}

/**
 * returns difference of most and least common entry in occur
 * does not include 0 entries
 */
int64_t diff(int64_t *occur, int n)
{
    int64_t least = -1, most = 0;
    for (int i = 0; i < n; i++)
    {
        if (least == -1 && occur[i])      {least = occur[i];}

        if (occur[i] > most)              {most = occur[i];}
        if (occur[i] < least && occur[i]) {least = occur[i];}
    }
    return (most - least);
}

/**
 * needs -std=c99
 */
int main()
{
    int i, n, first, newlocsize, maxindex, npair, index;
    int pairloc[MAX_PAIR], newloc[MAX_PAIR];
    int64_t *cnt, *newcnt;
    int64_t occur[BASE] = {0};
    char right;
    char input[MAX_IN], initial[MAX_IN], left[3], composition[3];
    Pair *pairs;

    // only consider pairs of letters
    maxindex = strtol("ZZ", NULL, BASE) + 1;

    cnt = calloc(maxindex, sizeof(*cnt));
    newcnt = calloc(maxindex, sizeof(*newcnt));
    if (!cnt || !newcnt) {return -1;}

    pairs = calloc(maxindex, sizeof(*pairs));
    if (!pairs) {return -2;}

    npair = 0;
    first = 1;
    composition[2] = '\0';
    while (fgets(input, sizeof(input), stdin))
    {
        if (input[0] == '\n') {first = 0;}
        else if (first)
        {
            strcpy(initial, input);
            n = strlen(initial) - 2;
            for (i = 0; i < n; i++)
            {
                // determine valid pairs in our initial state
                composition[0] = initial[i];
                composition[1] = initial[i + 1];
                index = strtol(composition, NULL, BASE);

                occur[initial[i] - 'A']++;

                cnt[index]++;
                pairloc[npair++] = index;
            }
            // last letter
            occur[initial[n] - 'A']++;
        }
        else
        {
            if (sscanf(input, "%s -> %c\n", left, &right) == 2)
            {
                index = strtol(left, NULL, BASE);

                strcpy(pairs[index].name, left);

                // AB -> C gives AC and BC
                composition[0] = left[0];
                composition[1] = right;
                pairs[index].decomp[0] = strtol(composition, NULL, BASE);

                composition[0] = right;
                composition[1] = left[1];
                pairs[index].decomp[1] = strtol(composition, NULL, BASE);

                pairs[index].out = right;

            }
            else {printf("malformed input\n"); return -2;}
        }
    }

    for (i = 0; i < STEPS; i++)
    {
        // pt1
        if (i == 10)
        {
            printf("pt1 - difference between most and least common: %lld\n", diff(occur, BASE));
        }

        newlocsize = 0;
        memset(newcnt, 0, maxindex * sizeof(*newcnt));
        memset(newloc, 0, sizeof(newloc));

        // for each pair we create two new decompositions
        // all done on a brand new set so that values are not updated mid operation
        for (n = 0; n < npair; n++)
        {
            index = pairs[pairloc[n]].decomp[0];
            if (!contain(newloc, newlocsize, index)) {newloc[newlocsize++] = index;}
            newcnt[index] += cnt[pairloc[n]];

            index = pairs[pairloc[n]].decomp[1];
            if (!contain(newloc, newlocsize, index)) {newloc[newlocsize++] = index;}
            newcnt[index] += cnt[pairloc[n]];

            occur[(int) pairs[pairloc[n]].out - 'A'] += cnt[pairloc[n]];
        }

        memcpy(cnt, newcnt, maxindex * sizeof(*cnt));
        memcpy(pairloc, newloc, sizeof(pairloc));
        npair = newlocsize;
    }

    printf("pt2 - difference between most and least common: %lld\n", diff(occur, BASE));

    free(pairs);
    free(newcnt);
    free(cnt);

    return 0;
}
